:Title: xmppdump
:Author: Andrew Biggs <balthorium@gmail.com> 
:Type: Informational 
:Status: Draft
:Revision: $Revision: 18396 $ 
:Last-Modified: $Date: 2010-10-07 13:22:12 -0700 (Thu, 07 Oct 2010) $ 
:Content-Type: text/x-rst

.. contents:: 

.. sectnum::    


What is ``xmppdump``?
=====================

The ``xmppdump`` application is a simple command line utility for tracing
network traffic, in the same spirit as ``tcpdump``.  Like ``tcpdump``, it is
based on ``libpcap``, with functionality similar to the ``tcpdump -X`` option
which provides a means for viewing the ascii text payload of TCP packets.  The 
``xmppdump`` utility, being geared specifically for XMPP, attempts to
provide a capture format better adapted to the XML based structure of the
protocol, which can facilitiate XMPP diagnostics and troubleshooting.  

How to Use ``xmppdump``
=======================

The ``xmppdump`` command line accepts one or more TCP port numbers on which to
capture network traffic.  When executed, ``xmppdump`` will capture all TCP
packets sent to or from any socket addresses involving those particular port
numbers.  For each packet, it will then write a textual summary of a select set
of TCP packet header fields, along with the xml-based payload of the packet (if
any).

For example, to use ``xmppdump`` to track XMPP traffic on ports 5222, 5269, and
7400, the command would be::

    xmppdump 5222 5269 7400

With this, all traffic on these ports will be captured and written to the
terminal, or alternatively streamed to a text file as with::

    xmppdump 5222 5269 7400 > xmpp.jcap

The following is an example of the format used by ``xmppdump`` when writing
captured packets to standard out::

	[tcp from="192.168.0.10:48720" to="192.168.0.10:5222" flags="PA" seqno="3835358166" ackno="3835695543" time="1286465969267" length="108" readable="true"]
	<iq type='get'
	     id='48'>
	    <query xmlns='jabber:iq:auth'>
	        <username>
	            mcast-client-agroup-0
	        </username>
	    </query>
	</iq>
	
	[tcp from="192.168.0.10:41261" to="192.168.0.10:7400" flags="PA" seqno="3837224097" ackno="3835391132" time="1286465969267" length="253" readable="true"]
	<route from='cm-1_jsmcp-1_xmppd-1@cm-1_jsmcp-1.jabber/00321887E'
	     to='mcast-client-agroup-0@my-xcp-10.local'
	     type='auth'>
	    <iq id='48'
	         type='get'
	         xml:lang='en-US'>
	        <query xmlns='jabber:iq:auth'>
	            <username>
	                mcast-client-agroup-0
	            </username>
	        </query>
	    </iq>
	</route>

A few points worth noting about ``xmppdump`` usage:

1. If you supply a port number, 5222 for example, all traffic going to or from
   any socket with that port number will be captured.  This includes both local
   and remote sockets, and all local interfaces.

2. Because ``xmppdump`` listens on the ``libpcap`` "any" device, it must be
   executed with root privileges.  Failing to do so will result in the error::

    Could not open device "any": socket: Operation not permitted

3. If the output you get from ``xmppdump`` shows many, or all, packets as 
   ``readable="false"``, it is likely that the traffic on that socket is either
   encrypted, or otherwise not text based.

4. To execute ``xmppdump``, your system must have the ``libpcap`` package
   installed.  You can verify that ``libpcap`` is installed with the commands::

    rpm -qa | grep pcap          # on rpm based systems (Red Hat, etc)
    dpkg-query -l | grep pcap    # on dpkg based systems (Ubuntu, etc)

5. The ``xmppdump`` source is really simple, possibly fewer lines than this
   howto.  if you need it to do something it currently does not support, adding
   features should be very straightforward.  It is built on ``libpcap``, so it 
   can be made to support all of the same filtering options as ``tcpdump``.

6. In its current form, ``xmppdump`` is limited to Linux systems with kernel
   versions of 2.2 or later.


How to Build ``xmppdump``
=========================

g++ xmppdump.cpp xmlpp.cpp -lpcap -o xmppdump

.. vim:set syntax=rest:

