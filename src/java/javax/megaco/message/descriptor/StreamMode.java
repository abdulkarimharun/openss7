// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) src/java/javax/megaco/message/descriptor/StreamMode.java <p>
 
 Copyright &copy; 2008-2015  Monavacon Limited <a href="http://www.monavacon.com/">&lt;http://www.monavacon.com/&gt;</a>. <br>
 Copyright &copy; 2001-2008  OpenSS7 Corporation <a href="http://www.openss7.com/">&lt;http://www.openss7.com/&gt;</a>. <br>
 Copyright &copy; 1997-2001  Brian F. G. Bidulock <a href="mailto:bidulock@openss7.org">&lt;bidulock@openss7.org&gt;</a>. <p>
 
 All Rights Reserved. <p>
 
 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license. <p>
 
 This program is distributed in the hope that it will be useful, but <b>WITHOUT
 ANY WARRANTY</b>; without even the implied warranty of <b>MERCHANTABILITY</b>
 or <b>FITNESS FOR A PARTICULAR PURPOSE</b>.  See the GNU Affero General Public
 License for more details. <p>
 
 You should have received a copy of the GNU Affero General Public License along
 with this program.  If not, see
 <a href="http://www.gnu.org/licenses/">&lt;http://www.gnu.org/licenses/&gt</a>,
 or write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA. <p>
 
 <em>U.S. GOVERNMENT RESTRICTED RIGHTS</em>.  If you are licensing this
 Software on behalf of the U.S. Government ("Government"), the following
 provisions apply to you.  If the Software is supplied by the Department of
 Defense ("DoD"), it is classified as "Commercial Computer Software" under
 paragraph 252.227-7014 of the DoD Supplement to the Federal Acquisition
 Regulations ("DFARS") (or any successor regulations) and the Government is
 acquiring only the license rights granted herein (the license rights
 customarily provided to non-Government users).  If the Software is supplied to
 any unit or agency of the Government other than DoD, it is classified as
 "Restricted Computer Software" and the Government's rights in the Software are
 defined in paragraph 52.227-19 of the Federal Acquisition Regulations ("FAR")
 (or any successor regulations) or, in the cases of NASA, in paragraph
 18.52.227-86 of the NASA Supplement to the FAR (or any successor regulations). <p>
 
 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See
 <a href="http://www.openss7.com/">http://www.openss7.com/</a>
 */

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMES   NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

javax.megaco.message.descriptor
Class StreamMode

java.lang.Object
  |
  +--javax.megaco.message.descriptor.StreamMode

   All Implemented Interfaces:
          java.io.Serializable

   Direct Known Subclasses:
          None
     _________________________________________________________________

   public class StreamMode
   extends java.lang.Object

   Stream mode constants used in package javax.megaco.message.descriptor.
   This defines the stream modes for the megaco package.

   See Also:
          Serialized Form
     _________________________________________________________________

   Field Summary
   static int M_STREAM_MODE_SEND_ONLY
              Identifies stream mode as send only.
   Its value shall be set to 0.
   static int M_STREAM_MODE_RECV_ONLY
              Identifies stream mode as receive only.
   Its value shall be set to 1.
   static int M_STREAM_MODE_SEND_RECV
              Identifies stream mode as "send receive".
   Its value shall be set to 2.
   static int M_STREAM_MODE_INACTIVE
              Identifies stream mode as "inactive".
   Its value shall be set to 3.
   static int M_STREAM_MODE_LOOPBACK
              Identifies stream mode as "loopback".
   Its value shall be set to 4.

                                                                   static

   StreamMode STREAM_MODE_SEND_ONLY
              Identifies a StreamMode object that constructs the class
   with the constant M_STREAM_MODE_SEND_ONLY.

                                                                   static

   StreamMode STREAM_MODE_RECV_ONLY
              Identifies a StreamMode object that constructs the class
   with the constant M_STREAM_MODE_RECV_ONLY.

                                                                   static

   StreamMode STREAM_MODE_SEND_RECV
              Identifies a StreamMode object that constructs the class
   with the constant M_STREAM_MODE_SEND_RECV.

                                                                   static

   StreamMode STREAM_MODE_INACTIVE
              Identifies a StreamMode object that constructs the class
   with the constant M_STREAM_MODE_SEND_INACTIVE.

                                                                   static

   StreamMode STREAM_MODE_LOOPBACK
              Identifies a StreamMode object that constructs the class
   with the constant M_STREAM_MODE_LOOPBACK.

   Constructor Summary
   StreamMode(int stream_mode)
              Constructs a class initialised with value stream mode as
   passed to it in the constructor.

   Method Summary
   int getStreamMode()
             Returns one of the allowed integer values defined as static
   fields in this class which shall identify one of the stream modes.

                                                                   static

   StreamMode getObject(int value)
           throw IllegalArgumentException()

             Returns reference of the StreamMode object that identifies
   the stream mode as value passed to this method.

                                                                   object

   readResolve()
              throws ObjectStreamException()

              This method is required to perform instance substitution
   during serialization.

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll,
   toString, wait

   Field Detail

  M_STREAM_MODE_SEND_ONLY

public static final int M_STREAM_MODE_SEND_ONLY

          Identifies stream mode to be send only.
          Its value shall be set to 0.
     _________________________________________________________________

  M_STREAM_MODE_RECV_ONLY

public static final int M_STREAM_MODE_RECV_ONLY

          Identifies stream mode to be receive only.
          Its value shall be set to 1.
     _________________________________________________________________

  M_STREAM_MODE_SEND_RECV

public static final int M_STREAM_MODE_SEND_RECV

          Identifies stream mode to be "send receive".
          Its value shall be set to 2.
     _________________________________________________________________

  M_STREAM_MODE_INACTIVE

public static final int M_STREAM_MODE_INACTIVE

          Identifies stream mode to be "inactive".
          Its value shall be set to 3.
     _________________________________________________________________

  M_STREAM_MODE_LOOPBACK

public static final int M_STREAM_MODE_LOOPBACK

          Identifies stream mode to be "loopback".
          Its value shall be set to 4.
     _________________________________________________________________

  STREAM_MODE_SEND_ONLY

public static final StreamMode STREAM_MODE_SEND_ONLY

          Identifies a StreamMode object that constructs the class with
          the constant M_STREAM_MODE_SEND_ONLY. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  STREAM_MODE_SEND_RECV

public static final StreamMode STREAM_MODE_SEND_RECV

          Identifies a StreamMode object that constructs the class with
          the constant M_STREAM_MODE_SEND_RECV. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  STREAM_MODE_RECV_ONLY

public static final StreamMode STREAM_MODE_RECV_ONLY

          Identifies a StreamMode object that constructs the class with
          the constant M_STREAM_MODE_RECV_ONLY. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  STREAM_MODE_INACTIVE

public static final StreamMode STREAM_MODE_INACTIVE

          Identifies a StreamMode object that constructs the class with
          the constant M_STREAM_MODE_INACTIVE. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  STREAM_MODE_LOOPBACK

public static final StreamMode STREAM_MODE_LOOPBACK

          Identifies a StreamMode object that constructs the class with
          the constant M_STREAM_MODE_LOOPBACK. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

   Constructor Detail

  StreamMode

private StreamMode(int stream_mode)

          Constructs an class that specifies the stream mode in the local
          control descriptor.
     _________________________________________________________________

   Method Detail

  getStreamMode

public int getStreamMode()

          This method returns one of the static field constants defined
          in this class.


        Returns:
                Returns an integer value that identifies the stream mode
                to be one of send only or receive only or send receive or
                inactive or loopback.
     _________________________________________________________________

  getObject

public static final StreamMode getObject(int value)
                throws IllegalArgumentException()

          Returns reference of the StreamMode object that identifies the
          stream mode as value passed to this method.


                Parameters:
                          value - It is one of the possible values of the
                static constant that this class provides.

        Returns:
                Returns reference of the StreamMode object.

        throws:
                IllegalArgumentException() - If the value passed to this
                method is invalid, then this exception is raised.
     _________________________________________________________________

  readResolve

private java.lang.Object readResolve()

        This method must be implemented to perform instance substitution
   during serialization. This method is required for reference
   comparison. This method if not implimented will simply fail each time
   we compare an Enumeration value against a de-serialized Enumeration
   instance.
     _________________________________________________________________
     _________________________________________________________________

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMES   NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

                  Copyright (C) 2001 Hughes Software Systems
       HUGHES SOFTWARE SYSTEMS and JAIN JSPA SIGNATORIES PROPRIETARY
        This document contains proprietary information that shall be
     distributed, routed or made available only within Hughes Software
       Systems and JAIN JSPA Signatory Companies, except with written
                   permission of Hughes Software Systems
             _________________________________________________

   22 December 2003


    If you have any comments or queries, please mail them to
    Jmegaco_hss@hss.hns.com
*/
