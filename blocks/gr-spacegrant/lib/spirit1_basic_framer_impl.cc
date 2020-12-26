/* -*- c++ -*- */
/*
 * Copyright 2018 <+YOU OR YOUR COMPANY+>.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "spirit1_basic_framer_impl.h"

#include <stdio.h>

namespace gr {
  namespace spacegrant {

    spirit1_basic_framer::sptr
    spirit1_basic_framer::make(pmt::pmt_t preamble_bytes, pmt::pmt_t sync_bytes, int length_len, int address_len, int control_len, int crc_mode)
    {
      return gnuradio::get_initial_sptr
        (new spirit1_basic_framer_impl(preamble_bytes, sync_bytes, length_len, address_len, control_len, crc_mode));
    }

    /*
     * The private constructor
     */
    spirit1_basic_framer_impl::spirit1_basic_framer_impl(pmt::pmt_t preamble_bytes, pmt::pmt_t sync_bytes, int length_len, int address_len, int control_len, int crc_mode)
      : gr::sync_block("spirit1_basic_framer",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(1, 1, sizeof(unsigned char)))
    {

        message_port_register_in(pmt::mp("in"));

  		  crc8_populate_msb(crc_table, 0x07);

    }

    /*
     * Our virtual destructor.
     */
    spirit1_basic_framer_impl::~spirit1_basic_framer_impl()
    {
    }

    //unpack packed (8 bits per byte) into bits, in LSbit order.
    //TODO: handle non-multiple-of-8 bit lengths (pad low)
    std::vector<unsigned char>
    spirit1_basic_framer_impl::unpack(std::vector<unsigned char> &data) {

        uint8_t lfsr_default[] = {0xFF, 0xE1, 0x1D, 0x9A, 0xED, 0x85, 0x33, 0x24, 0xEA};
        uint8_t lfsr[9];

        memcpy(lfsr,lfsr_default,9);

        std::vector<unsigned char> output(data.size()*8, 0);
        for(size_t i=0; i<data.size(); i++) {
            for(size_t j=0; j<8; j++) {
              int l = 7-j;
              uint8_t next_bit = (data[i] >> l) & 1;

              if(i > 7){ // After preamble/sync

                next_bit ^= (lfsr[0] >> 7);

                uint16_t lfsr_bit = ((lfsr[0] & 0x80) >> 7) ^ ((lfsr[5] & 0x80) >> 7);

                int k;
                for(k = 0; k < 8; k++){
                  lfsr[k] = (lfsr[k] << 1) & 0xFE;
                  lfsr[k] |= (lfsr[k+1] >> 7) & 0x01;
                }

                lfsr[8] = ((lfsr[8] << 1) & 0xFE) | lfsr_bit;

              }


              output[i*8+j] = next_bit;

            }
        }
        return output;
    }

    void spirit1_basic_framer_impl::crc8_populate_msb(uint8_t table[CRC8_TABLE_SIZE], uint8_t polynomial){
	    int i, j;
    	const uint8_t msbit = 0x80;
    	uint8_t t = msbit;

    	table[0] = 0;

    	for (i = 1; i < CRC8_TABLE_SIZE; i *= 2) {
    		t = (t << 1) ^ (t & msbit ? polynomial : 0);
	    	for (j = 0; j < i; j++)
    			table[i+j] = table[j] ^ t;
	    }
    }

    uint8_t spirit1_basic_framer_impl::crc8(const uint8_t table[CRC8_TABLE_SIZE], uint8_t *pdata, size_t nbytes, uint8_t crc)
    {
    	/* loop over the buffer data */
	    while (nbytes-- > 0)
		    crc = table[(crc ^ *pdata++) & 0xff];

    	return crc;
    }

    int
    spirit1_basic_framer_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {

      unsigned char *out = (unsigned char *) output_items[0];

        //send leftovers one chunk at a time.
        //it'd be more efficient to send as much as possible, i.e.
        //partial packets., but if we're to preserve tag boundaries
        //this is much, much simpler.
        int oidx = 0;
        while(d_leftovers.size() > 0) {


            if((size_t)noutput_items < (oidx+d_leftovers[0].size())){
                usleep(5000);
                return oidx;
            }
            memcpy(out+oidx, &d_leftovers[0][0], d_leftovers[0].size());
            //start tag
            /*add_item_tag(0,
                         nitems_written(0)+oidx,
                         d_frame_tag,
                         pmt::from_long(d_leftovers[0].size()),
                         d_me);*/
            oidx += d_leftovers[0].size();
            d_leftovers.erase(d_leftovers.begin());
        }

        //get PDU
        pmt::pmt_t msg;
        if(oidx == 0)
          msg = pmt::pmt_t(delete_head_blocking(pmt::mp("in")));
        else
          msg = pmt::pmt_t(delete_head_blocking(pmt::mp("in"), 10));

        if(msg.get() == NULL) return oidx;

        pmt::pmt_t len(pmt::car(msg)); //TODO for non-mult-8 nbits
        pmt::pmt_t blob(pmt::cdr(msg));

        if(!pmt::is_blob(blob))
            throw std::runtime_error("HDLC framer: PMT must be blob");

        uint8_t pre_sync[] = {0x01, 0x02, 0x03, 0x04, 0x88, 0x88, 0x88, 0x88};
        uint8_t post_sync[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};

        std::vector<unsigned char> pkt(pmt::blob_length(blob)+1+1+sizeof(pre_sync)+sizeof(post_sync));

        int packet_len = 0;

        // Add framing
        memcpy(&pkt[packet_len], pre_sync, sizeof(pre_sync));
        packet_len += sizeof(pre_sync);

        // Add length
        pkt[packet_len] = pmt::blob_length(blob);
        packet_len += 1;

        memcpy(&pkt[packet_len], (const unsigned char *) pmt::blob_data(blob), pmt::blob_length(blob));
        packet_len += pmt::blob_length(blob);

        //calc CRC
        uint8_t crc = crc8(crc_table, &pkt[sizeof(pre_sync)], pmt::blob_length(blob)+1, 0xFF);
        pkt[packet_len] = crc;
        packet_len += 1;

        memcpy(&pkt[packet_len], post_sync, sizeof(post_sync));
        packet_len += sizeof(post_sync);

        //unpack to LSb bits
        std::vector<unsigned char> pkt_bits = unpack(pkt);

        /*
        uint16_t k;
        for(k = 0; k < pkt_bits.size(); k++){
          if(pkt_bits[k] == 0) printf("0");
          else printf("1");
        }
        printf("\n");*/

        if((size_t)noutput_items < (oidx+pkt_bits.size())) {
            d_leftovers.insert(d_leftovers.end(), pkt_bits);
            return oidx;
        }

        //produce
        memcpy(out+oidx, &pkt_bits[0], pkt_bits.size());
        //start tag
        /*add_item_tag(0,
                     nitems_written(0)+oidx,
                     d_frame_tag,
                     pmt::from_long(pkt_bits.size()),
                     d_me);*/
        oidx += pkt_bits.size();

        //return # output bits
        return oidx;
    }

  } /* namespace spacegrant */
} /* namespace gr */
