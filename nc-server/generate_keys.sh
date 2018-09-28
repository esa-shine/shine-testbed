#!/bin/bash

ssh-keygen -b 2048 -t rsa -f ~/.ssh/id_rsa -q -N ""
cp ~/.ssh/id_rsa.pub id_rsa.pub
