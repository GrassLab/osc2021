FROM dockcross/linux-arm64


# Setup Rust cross build env
ENV DEFAULT_DOCKCROSS_IMAGE dockcross-linux-aarch64


ENV CARGO_HOME=/opt/.cargo
ENV RUSTUP_HOME=/opt/.rust
ENV PATH=$PATH:/opt/.cargo/bin

RUN curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- \
    -y \
    --default-host aarch64-unknown-linux-gnu \
    --target aarch64-unknown-linux-gnu
