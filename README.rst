Ping-pong eCos demo
===================

This is a repository for the ping-pong demo for the port of `eCos 3.0 RTOS <http://ecos.sourceware.org/>`_ for the `Toradex Colibri VF61 Freescale Vybrid Computer on Module <http://developer.toradex.com/product-selector/colibri-vf61>`_.

Compilation
-----------

The compilation can be done using the following steps:

.. code-block:: bash

   git clone https://github.com/antmicro/ecos-colibri-vf61-pingpong
   cd ecos-colibri-vf61-pingpong/src
   vi make.sh
   [use your favourite editor and set toolchain path]
   ./make.sh

This repository already contains a pre-compiled eCos kernel suitable for use with the ping-pong application.

If a custom eCos kernel is to be used instead, it can be compiled separately and pointed to by the BTPATH variable.

Running
-------

Use the following command to run the aplication from Linux userspace:

.. code-block:: bash

   mqxboot pingpong.bin 0x8f000400 0x0f000411
