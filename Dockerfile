FROM ubuntu:22.04

RUN apt-get update && \
    apt-get install -y software-properties-common wget && \
    wget -O /etc/apt/trusted.gpg.d/llvm.asc https://apt.llvm.org/llvm-snapshot.gpg.key && \
    apt-add-repository "deb http://apt.llvm.org/jammy/ llvm-toolchain-jammy-14 main" && \
    apt-get update && \
    apt-get install -y z3 python3-venv cmake git libz-dev unzip vim emacs valgrind gdb libgmp3-dev libtbb-dev llvm-14-dev clang-14 libclang-14-dev libboost-all-dev sqlite3 libsqlite3-dev libclang-rt-14-dev tree zip curl zsh && \
    rm -rf /var/lib/apt/lists/*

# Installing AFL++
RUN git clone https://github.com/AFLplusplus/AFLplusplus && \
    cd AFLplusplus && \
    git checkout v4.33c && \
    make source-only && \
    make install && \
    rm -r /AFLplusplus

# Installing IKOS
RUN git clone https://github.com/NASA-SW-VnV/ikos.git && \
    cd ikos && \
    git checkout v3.5 && \
    mkdir build && \
    cd build && \
    cmake -DCMAKE_CXX_COMPILER=clang++-14 -DCMAKE_INSTALL_PREFIX=/usr/local -DLLVM_CONFIG_EXECUTABLE=/usr/bin/llvm-config-14 .. && \
    make && \
    make install && \
    rm -r /ikos

ENV PYTHONPATH=/usr/local/lib/python3.10/site-packages

# Installing Souffle
RUN apt-get update && \
    apt-get -y install autoconf automake bison build-essential clang doxygen flex g++ git libffi-dev libncurses5-dev libtool libsqlite3-dev make mcpp sqlite zlib1g-dev && \
    rm -rf /var/lib/apt/lists/* && \
    wget https://github.com/souffle-lang/souffle/archive/refs/tags/2.5.zip && \
    unzip 2.5.zip && \
    cd souffle-2.5 && \
    cmake -S . -B build && \
    cmake --build build -j8 && \
    cd build && \
    make && \
    make install && \
    cd ../../ && \
    rm 2.5.zip

# Installing KLEE (unused due to UClibc not supporting ARM)
RUN apt-get update && \
    apt-get install -y libtcmalloc-minimal4 libgoogle-perftools-dev && \
    rm -rf /var/lib/apt/lists/* && \
    git clone https://github.com/klee/klee && \
    cd klee && \
    git checkout v3.1 && \
    mkdir build && \
    cd build && \
    cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DENABLE_KLEE_ASSERTS=OFF \
    -DENABLE_SOLVER_Z3=ON \
    -DENABLE_UNIT_TESTS=OFF \
    -DENABLE_SYSTEM_TESTS=OFF \
    .. && \
    make install && \
    rm -rf /klee

# Install Zsh
RUN chsh -s /usr/bin/zsh
RUN sh -c "$(curl -fsSL https://raw.github.com/ohmyzsh/ohmyzsh/master/tools/install.sh)"
COPY zshrc /root/.zshrc

# Install Gemini CLI
RUN curl -fsSL https://deb.nodesource.com/setup_22.x | bash - && \
    apt-get install -y nodejs && \
    npm install -g @google/gemini-cli

# Set-up symlinks for LLVM-14 toolchain.
RUN rm -rf /usr/bin/clang /usr/bin/clang++ /usr/bin/llvm-config /usr/bin/opt && \
    ln -s /usr/bin/clang-14 /usr/bin/clang && \
    ln -s /usr/bin/clang++-14 /usr/bin/clang++ && \
    ln -s /usr/bin/llvm-config-14 /usr/bin/llvm-config && \
    ln -s /usr/bin/opt-14 /usr/bin/opt

ENV CC=clang
ENV CXX=clang++
