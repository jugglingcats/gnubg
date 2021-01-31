FROM alpine

RUN apk add --no-cache alpine-sdk glib-dev bison flex automake autoconf \
    && git clone https://github.com/jugglingcats/gnubg.git \
    && cd gnubg \
    && aclocal && automake && ./configure --with-python=no \
    && make

