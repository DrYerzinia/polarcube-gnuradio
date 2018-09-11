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

#include <gnuradio/io_signature.h>
#include "message_debug_impl.h"

#include <cctype>
#include <cstdio>

namespace gr {
  namespace spacegrant {

    message_debug::sptr
    message_debug::make()
    {
      return gnuradio::get_initial_sptr
        (new message_debug_impl());
    }

    void
    message_debug_impl::print_pdu(pmt::pmt_t pdu)
    {

        pmt::pmt_t meta = pmt::car(pdu);
        pmt::pmt_t vector = pmt::cdr(pdu);
        size_t len = pmt::blob_length(vector);
        size_t offset(0);
        const uint8_t* d = (const uint8_t*) pmt::uniform_vector_elements(vector, offset);

        printf("Size: %zu\n", len);
        printf("\nData: \n");
        int i = 0;
        while(i < len){
                int j;
                for(j = 0; j < 20; j++){
                        if(i < len){
                                if(isprint(d[i])) putchar(d[i]);
                                else printf(".");
                        } else putchar(' ');
                        i++;
                }
                i -= 20;
                printf("  |  ");
                for(j = 0; j < 20; j++){
                        if(i < len){
                                unsigned char val = d[i];
                                printf(" %02X", (int)val);
                                i++;
                        }
                }
                putchar('\n');
        }
        fflush(stdout);

    }

    /*
     * The private constructor
     */
    message_debug_impl::message_debug_impl()
      : gr::block("message_debug",
        io_signature::make(0, 0, 0),
        io_signature::make(0, 0, 0))
    {
      message_port_register_in(pmt::mp("pdu"));
      set_msg_handler(pmt::mp("pdu"), boost::bind(&message_debug_impl::print_pdu, this, _1));
    }

    /*
     * Our virtual destructor.
     */
    message_debug_impl::~message_debug_impl()
    {
    }

  } /* namespace spacegrant */
} /* namespace gr */

