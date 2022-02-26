FROM fedora:33

RUN \
  dnf update -y && \ 
  dnf install -y unix2dos m4 wget xz gcc gcc-c++ gmp-devel mpfr-devel libmpc-devel make file texinfo srecord git libdsk java-11-openjdk unzip zip man vim-common diffutils && \
  dnf clean all
RUN \
  mkdir /opt/gas && \
  cd /opt/gas && \
  wget --quiet https://ftp.gnu.org/gnu/binutils/binutils-2.33.1.tar.xz && \
  wget --quiet https://ftp.gnu.org/pub/gnu/gcc/gcc-9.2.0/gcc-9.2.0.tar.xz && \
  xz -d binutils-2.33.1.tar.xz && \
  xz -d gcc-9.2.0.tar.xz && \
  tar xf binutils-2.33.1.tar && \
  tar xf gcc-9.2.0.tar
RUN \
  cd /opt/gas && \
  mkdir binutils && \
  cd binutils && \
  ../binutils-2.33.1/configure --target=m68k-elf && \
  make -j8 && \
  make install
RUN \
  cd /opt/gas && \
  mkdir gcc && \
  cd gcc && \
  ../gcc-9.2.0/configure --target=m68k-elf --enable-languages=c --disable-libssp && \
  make -j8 && \
  make install && \
  mkdir /opt/work && \
  echo "cd /opt/work" >> ~/.bashrc
