FROM ubuntu:xenial as base

ARG BUILD_REPO
ARG MAIN_SUBDOMAIN=www
ARG IMAGE_SUBDOMAIN=images
ARG ROOTDIR=/home/httpd/www
ARG IMAGEROOTDIR=/home/httpd/images
ARG LOCAL_INSTALL_DIR=/usr/local/share/www
ARG BUILD_RELEASE_NUMBER=0.0.1

ARG GIT_FOLDER=/backend

LABEL maintainer="ivan.kuchin@gmail.com"
LABEL org.opencontainers.image.source="https://github.com/IvanKuchin/${BUILD_REPO}"

###########################################
# check if all mandatory args are defined #
###########################################
RUN [ -z "$BUILD_REPO" ] && echo "BUILD_REPO is required" && exit 1 || true
RUN echo repository is $BUILD_REPO

###########################
# install basic utilities #
###########################
RUN apt update \
    && apt install --no-install-recommends --no-install-suggests -y \
           ca-certificates \
           tzdata \
           locales \
           gettext \
    && rm -rf /var/lib/apt/lists/*

####################
# install timezone #
####################
#RUN apt-get install --no-install-recommends --no-install-suggests -y tzdata \
#    && rm -rf /var/lib/apt/lists/*
RUN cp /usr/share/zoneinfo/Europe/Moscow /etc/localtime
RUN echo "Europe/Moscow" > /etc/timezone
RUN date

##############################
# install additional locales #
##############################
#RUN apt-get install --no-install-recommends --no-install-suggests -y locales gettext 
#    && rm -rf /var/lib/apt/lists/*
RUN locale-gen ru_RU.utf8
RUN locale-gen en_US.utf8
RUN update-locale LC_ALL=en_US.UTF8
# RUN export LC_ALL=en_US.utf8

############################################################################################


FROM base as build

ARG BUILD_REPO
ARG MAIN_SUBDOMAIN
ARG IMAGE_SUBDOMAIN
ARG ROOTDIR
ARG IMAGEROOTDIR
ARG LOCAL_INSTALL_DIR
ARG BUILD_RELEASE_NUMBER

########################
# install dependencies #
########################
RUN apt update \
    && apt-get install --no-install-recommends --no-install-suggests -y software-properties-common \
    && add-apt-repository -y ppa:jonathonf/ffmpeg-4 \
    && apt update \
    && apt-get install --no-install-recommends --no-install-suggests -y \ 
        build-essential \
        file \
        cmake \
        gsfonts \
        graphicsmagick-libmagick-dev-compat \
        libavcodec-dev \
        libavformat-dev \
        libavutil-dev \
        libcurl4-openssl-dev \
        libgraphicsmagick++-dev \
        libhpdf-dev \
        libmaxminddb-dev \
        libmysqlclient-dev \
        libssl-dev \
        libwebp-dev \
        libarchive-dev \
        rapidjson-dev \
        pkg-config \
        git \
        wget \
    && apt-get clean

###############
# pull PI src #
###############
COPY . ${GIT_FOLDER}
WORKDIR ${GIT_FOLDER}
RUN git clone --depth 1 https://github.com/IvanKuchin/backend_pi.git ./src/pi

######################################################
# pull PD src                                        #
# assume the repo name is xxxxxx-admin               #
# below command cuts out "dash" and everything after #
######################################################
RUN git clone --depth 1 https://github.com/IvanKuchin/${BUILD_REPO%-*}.git /tmp/${BUILD_REPO}/
RUN cp -R /tmp/${BUILD_REPO}/src/pd ./src

######################
# libxl installation #
######################
RUN if test -f "./libxl-3.8.4.0/lib64/libxl.so"; then \
       cp ./libxl-3.8.4.0/lib64/libxl.so /usr/local/lib/ \
       && ldconfig; \
    fi

#############################
# libwkhtmltox installation #
#############################
RUN if test -d "./wkhtmltox"; then \
       wget https://github.com/wkhtmltopdf/wkhtmltopdf/releases/download/0.12.5/wkhtmltox_0.12.5-1.trusty_amd64.deb \
       && apt install --no-install-recommends --no-install-suggests -y ./wkhtmltox*.deb; \
    fi

##################
# save commit id #
##################
RUN git rev-parse HEAD > ./supplemental/git/git_commit_id

##################### 
# build from source #
##################### 
RUN mkdir build
RUN cd build \ 
    && cmake \
        -DPROJECT_NAME=${BUILD_REPO} \
        -DMAIN_SUBDOMAIN=${MAIN_SUBDOMAIN} \
        -DIMAGE_SUBDOMAIN=${IMAGE_SUBDOMAIN} \
        -DROOTDIR=${ROOTDIR} \
        -DIMAGEROOTDIR=${IMAGEROOTDIR} \
        -DLOCAL_INSTALL_DIR=${LOCAL_INSTALL_DIR} \
        -DBUILD_RELEASE_NUMBERS=${BUILD_RELEASE_NUMBERS} \
        .. \
    && make -j2 package

############################################################################################

FROM base as app

ARG BUILD_REPO
ARG MAIN_SUBDOMAIN
ARG IMAGE_SUBDOMAIN
ARG ROOTDIR
ARG IMAGEROOTDIR
ARG LOCAL_INSTALL_DIR

WORKDIR ${GIT_FOLDER}

####################
# copy from build  #
####################
COPY --from=build ${GIT_FOLDER}/build/${BUILD_REPO}-infra.deb ./
COPY --from=build ${GIT_FOLDER}/build/${BUILD_REPO}-admin.deb ./
COPY --from=build ${GIT_FOLDER}/supplemental/apache2/ /etc/apache2/

###############
# copy libxl  #
###############
COPY --from=build /usr/local/lib/libxl* /usr/local/lib/

###################
# copy wkhtmltox  #
###################
COPY --from=build ${GIT_FOLDER}/wkhtmltox*.deb ./

####################
# install binaries #
####################
RUN apt update \
    && apt-get install --no-install-recommends --no-install-suggests -y \
               software-properties-common \
               ssmtp \
               apache2 \
    && apt install  --no-install-recommends --no-install-suggests -y ./*.deb \
    && rm -rf /var/lib/apt/lists/*

#########################
# configure mail sender #
#########################
RUN sed -i "s/#rewriteDomain=/rewriteDomain=${BUILD_REPO}/g" /etc/ssmtp/ssmtp.conf

#####################
# configure apache2 #
#####################
RUN mkdir -p ${IMAGEROOTDIR}
RUN mkdir ${ROOTDIR}/logs/

RUN a2enmod cgi \
    && a2enmod rewrite \
    && a2enmod expires \
    && a2enconf mod_expires \
    && a2dissite `ls /etc/apache2/sites-enabled/` \
    && a2ensite *www

############################################################
## forward request and error logs to docker log collector ##
############################################################
RUN ln -sf /dev/stdout ${ROOTDIR}/logs/access.log \
 && ln -sf /dev/stderr ${ROOTDIR}/logs/error.log

####################################################################
# change folders to proper owner                                   #
####################################################################
RUN chown -R www-data:www-data ${ROOTDIR} \
 && chown -R www-data:www-data ${IMAGEROOTDIR} \
 && chown -R www-data:www-data ${LOCAL_INSTALL_DIR}

EXPOSE 80

CMD ["apachectl", "-D", "FOREGROUND", ""]

