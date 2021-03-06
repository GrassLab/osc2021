FROM dockcross/linux-arm64


# Setup Rust cross build env
ENV DEFAULT_DOCKCROSS_IMAGE dockcross-linux-aarch64

ARG TARGET_TRIPLE=aarch64-unknown-none-softfloat

ENV CARGO_HOME=/opt/.cargo
ENV RUSTUP_HOME=/opt/.rust
ENV PATH=$PATH:/opt/.cargo/bin

RUN curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- \
    -y \
    --default-toolchain nightly \
    --target aarch64-unknown-none-softfloat

# RUN rustup target add aarch64-unknown-none-softfloat

# https://github.com/rust-embedded/cargo-binutils
RUN cargo install \
    cargo-binutils  \
    rustfilt \
    && rustup component add llvm-tools-preview

RUN chmod -R 777 ${CARGO_HOME}
