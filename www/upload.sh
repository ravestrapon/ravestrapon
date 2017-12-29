#!/bin/bash

RAVE_STRAPON_LOCAL_DIR="."
RAVE_STRAPON_SSH_KEY_DIR="${HOME}/.ssh/ravestrapon"
USERNAME="cmooney3_ravestrapon"
SSH_SERVER="ssh.phx.nearlyfreespeech.net"
RAVE_STRAPON_REMOTE_DIR="/home/public"

rsync -vazh --delete --delete-excluded \
  -e "ssh -i \"${RAVE_STRAPON_SSH_KEY_DIR}/id_rsa\"" \
  --exclude "upload.sh" --exclude "*.swp" --exclude "*.swo" \
  "${RAVE_STRAPON_LOCAL_DIR}/" \
  "${USERNAME}@${SSH_SERVER}:${RAVE_STRAPON_REMOTE_DIR}/"
