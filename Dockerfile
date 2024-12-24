FROM public.ecr.aws/lambda/nodejs:latest

RUN microdnf install -y git bison flex automake gcc make glib2-static.x86_64 which
RUN git clone https://github.com/jugglingcats/gnubg.git
WORKDIR gnubg
RUN git checkout tags/v1.2.0 -b build
RUN chmod +x configure install-sh make* mk*
RUN aclocal
RUN automake
RUN autoreconf
RUN ./configure --with-python=no
RUN make
RUN make install

RUN microdnf install -y gdb