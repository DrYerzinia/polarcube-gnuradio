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
#include "spirit1_basic_deframer_impl.h"

#include <stdio.h>

#define CRC8_TABLE_SIZE 256

uint8_t crc_table[CRC8_TABLE_SIZE];

void crc8_populate_msb(uint8_t table[CRC8_TABLE_SIZE], uint8_t polynomial){
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

uint8_t crc8(const uint8_t table[CRC8_TABLE_SIZE], uint8_t *pdata, size_t nbytes, uint8_t crc)
{
	/* loop over the buffer data */
	while (nbytes-- > 0)
		crc = table[(crc ^ *pdata++) & 0xff];

	return crc;
}

namespace gr {
  namespace spacegrant {

    spirit1_basic_deframer::sptr
    spirit1_basic_deframer::make(pmt::pmt_t preamble_bytes, pmt::pmt_t sync_bytes, int length_len, int address_len, int control_len, int crc_mode)
    {
      return gnuradio::get_initial_sptr
        (new spirit1_basic_deframer_impl(preamble_bytes, sync_bytes, length_len, address_len, control_len, crc_mode));
    }

    /*
     * The private constructor
     */
    spirit1_basic_deframer_impl::spirit1_basic_deframer_impl(pmt::pmt_t preamble_bytes, pmt::pmt_t sync_bytes, int length_len, int address_len, int control_len, int crc_mode)
      : gr::sync_block("spirit1_basic_deframer",
              gr::io_signature::make(1, 1, sizeof(unsigned char)),
              gr::io_signature::make(0, 0, 0))
    {

      message_port_register_out(pmt::mp("out"));

      bit_counter = 0;
      state = SYNC_PREAMBLE;

      memset(packet_data_buffer, 0, 300);

      d_length_len = length_len;

		  crc8_populate_msb(crc_table, 0x07);

      uint8_t default_syncronizer[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

      memcpy(syncronizer, default_syncronizer, 8);

    }



    /*
     * Our virtual destructor.
     */
    spirit1_basic_deframer_impl::~spirit1_basic_deframer_impl()
    {
    }

		#include <arpa/inet.h>
		#include <sys/socket.h>
		#include <unistd.h>

		uint16_t checksum(void* vdata, size_t length){
		    // Cast the data pointer to one that can be indexed.
		    char* data=(char*)vdata;

		    // Initialise the accumulator.
		    uint32_t acc=0xffff;

		    // Handle complete 16-bit blocks.
		    for (size_t i=0;i+1<length;i+=2) {
		        uint16_t word;
		        memcpy(&word,data+i,2);
		        acc+=ntohs(word);
		        if (acc>0xffff) {
		            acc-=0xffff;
		        }
		    }

		    // Handle any partial block at the end of the data.
		    if (length&1) {
		        uint16_t word=0;
		        memcpy(&word,data+length-1,1);
		        acc+=ntohs(word);
		        if (acc>0xffff) {
		            acc-=0xffff;
		        }
		    }

		    // Return the checksum in network byte order.
		    return htons(~acc);
		}

    int
    spirit1_basic_deframer_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {

      const unsigned char *in = (const unsigned char *) input_items[0];

      uint8_t pre_sync_len = 4;
      uint8_t pre_sync[] = {0x88, 0x88, 0x88, 0x88};
			// 10001000100010001000100010001000
      //uint8_t pre_sync[] = {0xAA, 0xAA, 0xAA, 0xAA, 0x88, 0x88, 0x88, 0x88};
			// 1010101010101010101010101010101010001000100010001000100010001000
      //uint8_t pre_sync[] = {0x55, 0x55, 0x55, 0x55, 0x77, 0x77, 0x77, 0x77};

      uint8_t lfsr_default[] = {0xFF, 0xE1, 0x1D, 0x9A, 0xED, 0x85, 0x33, 0x24, 0xEA};

      uint8_t rx_bit;
      int n=0;
      while (n < noutput_items) {
        rx_bit = *in;

        if(state != SYNC_PREAMBLE){

          rx_bit ^= (lfsr[0] >> 7);

          uint16_t lfsr_bit = ((lfsr[0] & 0x80) >> 7) ^ ((lfsr[5] & 0x80) >> 7);

          int i;
          for(i = 0; i < 8; i++){
            lfsr[i] = (lfsr[i] << 1) & 0xFE;
            lfsr[i] |= (lfsr[i+1] >> 7) & 0x01;
          }

          lfsr[8] = ((lfsr[8] << 1) & 0xFE) | lfsr_bit;

        }

        switch(state){
          case SYNC_PREAMBLE:
            {
              /*
              int byte = bit_counter / 8;
              int bit = bit_counter % 8;

              uint8_t expected_bit = ((pre_sync[byte] >> (7-bit)) & 0x01);

              if(rx_bit == expected_bit){
                bit_counter++;
              } else {
                bit_counter = 0;
              }
              */

              // Shift syncronizer 1 bit
              int i;
              for(i = 0; i < (pre_sync_len-1); i++){
                syncronizer[i] = (syncronizer[i] << 1) & 0xFE;
                syncronizer[i] |= (syncronizer[i+1] >> 7) & 0x01;
              }

              // Add new bit to syncronizer
              syncronizer[(pre_sync_len-1)] = ((syncronizer[(pre_sync_len-1)] << 1) & 0xFE) | rx_bit;

              /*if(bit_counter > 10){
                printf("%d\n", bit_counter);
              }*/

              //if(bit_counter == 8*8){

              // check to see if we match the preamble sync pattern
              if(memcmp(syncronizer, pre_sync, pre_sync_len) == 0){

                bit_counter = 0;
                printf("MATCH!\n");
                state = LENGTH;
                packet_length = 0;
                computed_crc = 0xFF;

                memcpy(lfsr,lfsr_default,9);

              }
            }
            break;
          case LENGTH:
            {
              packet_length |= (rx_bit << (d_length_len - bit_counter - 1));
              bit_counter++;
              if(bit_counter == d_length_len){
                //printf("Length: %d\n", packet_length);
                bit_counter = 0;
                state = PAYLOAD;
                uint8_t p_len = packet_length;
                computed_crc = crc8(crc_table, &p_len, 1, computed_crc);
              }
            }
            break;
          case PAYLOAD:
            {

              int byte = bit_counter / 8;
              int bit = bit_counter % 8;

              packet_data_buffer[byte] |=  (rx_bit << (7-bit));

              bit_counter++;

              if(bit_counter == packet_length*8){

                bit_counter = 0;
                crc = 0;
                state = CRC;

                computed_crc = crc8(crc_table, packet_data_buffer, packet_length, computed_crc);

              }

            }
            break;
          case CRC:
            {

              int byte = bit_counter / 8;
              int bit = bit_counter % 8;

              crc |=  (rx_bit << (7-bit));

              bit_counter++;

              if(bit_counter == 8){

                if(crc == computed_crc){

						      uint8_t packet[300];

						      // Recreate IP header if was stripped
						      if(packet_data_buffer[0] < 6){
						        uint8_t header[] = {0x45, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00,
						                            0x40, 0x11, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01,
						                            0x01, 0x01, 0x01, 0x02, 0x8b, 0xc1, 0x8b, 0xc1,
						                            0x00, 0x00, 0x00, 0x00};

						        memcpy(packet+28, packet_data_buffer, packet_length);
						        memcpy(packet, &header, 28);

						        packet[2] = ((packet_length+28) & 0xff00)>>8;
						        packet[3] = (packet_length+28) & 0xff;

						        uint16_t ics = checksum(packet, 20);
						        packet[11] = (ics & 0xff00)>>8;
						        packet[10] = ics & 0xff;

						        packet[24] = ((packet_length+8) & 0xff00)>>8;
						        packet[25] = (packet_length+8) & 0xff;
						        //uint16_t cs = udp_checksum((uint16_t*)packet+20, len-9, (uint16_t*)inet_addr("1.1.1.1"), (uint16_t*)inet_addr("1.1.1.2"));
						        //packet[27] = (cs & 0xff00)>>8;
						        //packet[26] = cs & 0xff;

										packet_length += 28;

						      } else {
						        memcpy(packet, packet_data_buffer, packet_length);
						      }

		              pmt::pmt_t pdu(pmt::cons(pmt::PMT_NIL, pmt::make_blob(packet, packet_length)));
		              message_port_pub(pmt::mp("out"), pdu);

                } else {

									printf("CRC Fail");

								}

                memset(packet_data_buffer, 0, 300);
                //printf("CRC         : %02X\n", crc);
                //printf("CRC Computed: %02X\n", computed_crc);

                bit_counter = 0;
                state = SYNC_PREAMBLE;

              }

            }
            break;

        }

        // Look at next bit
        in++;
        n++;

      }

      // Tell runtime system how many output items we produced.
      return n;
    }

  } /* namespace spacegrant */
} /* namespace gr */
