FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update
RUN apt-get upgrade -y
RUN apt-get install -y apt-utils build-essential vim nano net-tools
RUN apt-get install -y scons libpcre2-32-0 python3-tds tdsodbc python3-pyqt5 cmake valgrind
RUN apt-get install -y python3-pyodbc libqt5core5a freetds-common freetds-dev make libssl-dev
RUN apt-get install -y python-pip-whl python3-pip libnuma-dev libnuma1 s3cmd libtool awscli libpcre2-16-0
RUN apt-get install -y git

# RUN pip install libnuma-dev
# RUN pip install libnuma1
RUN pip install s3cmd
RUN pip install libtool
RUN pip install githubrelease
# RUN pip install libpcre2-32-0
# RUN pip install libpcre2-16-0

RUN export USER=root
RUN export GITHUB_USER=gbabar-galaxy
RUN export GITHUB_COMPANY=BlueFireCapital
RUN export GITHUB_TOKEN=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
RUN export NEBULA_HOME=/tmp/$USER/dist/nebula/

RUN git clone https://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@github.com/BlueFireCapital/Nebula.git
RUN git clone https://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@github.com/BlueFireCapital/JaneStGw.git

