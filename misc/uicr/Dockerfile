FROM node:lts-alpine

WORKDIR /root

RUN yarn add nrf-intel-hex
ADD uicr.js .

CMD [ "node", "uicr.js" ]
