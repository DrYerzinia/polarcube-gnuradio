/* -*- c++ -*- */
/*
 * Copyright 2017 <+YOU OR YOUR COMPANY+>.
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

#include <stdio.h>
#include <gnuradio/io_signature.h>
#include "ax25_pdu_unpacker_impl.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

uint16_t udp_checksum(uint16_t buff[], uint16_t len_udp, uint16_t src_addr[],uint16_t dest_addr[])
{
  uint16_t prot_udp=17;
  uint16_t padd=0;
  uint16_t word16;
  uint32_t sum;
  int i;
  
  // Find out if the length of data is even or odd number. If odd,
  // add a padding byte = 0 at the end of packet
  if (len_udp%2==1){
    padd=1;
    buff[len_udp]=0;
  }
  
  //initialize sum to zero
  sum=0;
  
  // make 16 bit words out of every two adjacent 8 bit words and 
  // calculate the sum of all 16 vit words
  for (i=0;i<len_udp+padd;i=i+2){
    word16 =((buff[i]<<8)&0xFF00)+(buff[i+1]&0xFF);
    sum = sum + (unsigned long)word16;
  }
  // add the UDP pseudo header which contains the IP source and destinationn addresses
  for (i=0;i<4;i=i+2){
    word16 =((src_addr[i]<<8)&0xFF00)+(src_addr[i+1]&0xFF);
    sum=sum+word16;
  }
  for (i=0;i<4;i=i+2){
    word16 =((dest_addr[i]<<8)&0xFF00)+(dest_addr[i+1]&0xFF);
    sum=sum+word16; 
  }
  // the protocol number and the length of the UDP packet
  sum = sum + prot_udp + len_udp;

  // keep only the last 16 bits of the 32 bit calculated sum and add the carries
  while (sum>>16)
    sum = (sum & 0xFFFF)+(sum >> 16);
  
  // Take the one's complement of sum
  sum = ~sum;

  return ((uint16_t) sum);
}


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

namespace gr {
  namespace spacegrant {

    ax25_pdu_unpacker::sptr
    ax25_pdu_unpacker::make(int addr_filt, const std::string &src_addr, const std::string &dest_addr)
    {
      return gnuradio::get_initial_sptr
        (new ax25_pdu_unpacker_impl(addr_filt, src_addr, dest_addr));
    }

    /*
     * The private constructor
     */
    ax25_pdu_unpacker_impl::ax25_pdu_unpacker_impl(int addr_filt, const std::string &src_addr, const std::string &dest_addr)
      : gr::block("ax25_pdu_unpacker",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(0, 0, 0)),
              d_addr_filt(addr_filt), d_src_addr(src_addr), d_dest_addr(dest_addr)
    {
      message_port_register_out(pmt::mp("pdu_out"));
      message_port_register_in(pmt::mp("pdu_in"));
      set_msg_handler(pmt::mp("pdu_in"), boost::bind(&ax25_pdu_unpacker_impl::pdu_in, this, _1));
    }

    /*
     * Our virtual destructor.
     */
    ax25_pdu_unpacker_impl::~ax25_pdu_unpacker_impl()
    {
    }

    void
    ax25_pdu_unpacker_impl::pdu_in(pmt::pmt_t pdu){

      pmt::pmt_t vector = pmt::cdr(pdu);
      size_t len = pmt::blob_length(vector);
      size_t offset(0);
      const uint8_t* d = (const uint8_t*) pmt::uniform_vector_elements(vector, offset);

      // Get destination address
      char buf[8], buf2[20];
      int end_call = -1;

      for(int i = 0; i < 7; i++){
        char c = (signed char)((unsigned char)(d[i])>>1);
        if(c == 0x20 | i == 6) c = 0x0;
        buf[i] = c;
      }
      if(d_addr_filt == ADDR_FILT_DEST || d_addr_filt == ADDR_FILT_BOTH){
        sprintf(buf2, "%s-%d", buf, ((d[6] >> 1) & 0x0F));
        if(d_dest_addr.compare(std::string(buf2)) != 0) return;
      }

      // Get source address
      for(int i = 7; i < 14; i++){
        char c = (signed char)((unsigned char)(d[i])>>1);
        if(c == 0x20 | i == 13) c = 0x0;
        buf[i-7] = c;
      }
      if(d_addr_filt == ADDR_FILT_SRC || d_addr_filt == ADDR_FILT_BOTH){
        sprintf(buf2, "%s-%d", buf, ((d[13] >> 1) & 0x0F));
        if(d_src_addr.compare(std::string(buf2)) != 0) return;
      }

      // check end of address
      // check control
      // check PID

      ///////////////////////////////////////////////////////////////

      uint8_t packet[300];

      // Recreate IP header if was stripped
      if(d[16] == 0x46){
        uint8_t header[] = {0x45, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00,
                            0x40, 0x11, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01,
                            0x01, 0x01, 0x01, 0x02, 0x8b, 0xc1, 0x8b, 0xc1,
                            0x00, 0x00, 0x00, 0x00};

        memcpy(packet+27, d+16, len-16);
        memcpy(packet, &header, 28);

        packet[2] = ((len+11) & 0xff00)>>8;
        packet[3] = (len+11) & 0xff;

        uint16_t ics = checksum(packet, 20);
        packet[11] = (ics & 0xff00)>>8;
        packet[10] = ics & 0xff; 

        packet[24] = ((len-17) & 0xff00)>>8;
        packet[25] = (len-17) & 0xff;
        //uint16_t cs = udp_checksum((uint16_t*)packet+20, len-9, (uint16_t*)inet_addr("1.1.1.1"), (uint16_t*)inet_addr("1.1.1.2"));
        //packet[27] = (cs & 0xff00)>>8;
        //packet[26] = cs & 0xff; 

      } else {
        memcpy(packet, d+16, len);
      }

      message_port_pub(pmt::mp("pdu_out"), pmt::cons(pmt::PMT_NIL, pmt::init_u8vector(len+11, (const uint8_t *) &packet)));

    }

  } /* namespace spacegrant */
} /* namespace gr */
