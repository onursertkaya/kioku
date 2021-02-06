FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Europe/Stockholm

RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone \
    && \
    apt-get update && apt-get install -y \
        lsb-release wget software-properties-common ca-certificates \
        curl wget \
        git \
        python3 python3-pip python3-dev \
        build-essential \
        cmake \
        binutils \
        valgrind \
        linux-tools-$(uname -r) linux-tools-generic \
    && \
    python3 -m pip install --no-cache-dir \
        black \
        cpplint \
        flake8 isort pylint pydocstyle \
        mypy \
        hypothesis \
        jsonschema \
        pyfakefs \
    && \
    wget https://apt.llvm.org/llvm.sh && chmod +x llvm.sh && ./llvm.sh all && rm llvm.sh \
    && \
    rm -rf /var/lib/apt/lists/*
