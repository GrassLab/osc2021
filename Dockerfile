FROM dockcross/linux-arm64


# Setup Rust cross build env
ENV DEFAULT_DOCKCROSS_IMAGE dockcross-linux-aarch64

ARG TARGET_TRIPLE=aarch64-unknown-linux-gnu

ENV CARGO_HOME=/opt/.cargo
ENV RUSTUP_HOME=/opt/.rust
ENV PATH=$PATH:/opt/.cargo/bin

RUN curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- \
    -y \
    --profile minimal \
    --default-toolchain nightly \
    --target ${TARGET_TRIPLE}

RUN rustup component add rust-src
RUN cargo install xargo

RUN chmod -R 777 ${CARGO_HOME}

#  && xargo build --target aarch64-unknown-linux-gnu



