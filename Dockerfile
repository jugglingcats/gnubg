FROM alpine

RUN apk add --no-cache alpine-sdk bison flex
RUN git clone https://github.com/jugglingcats/gnubg.git
WORKDIR ./gnubg
RUN ./configure --with-python=no
RUN make
