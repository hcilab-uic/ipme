#!/bin/bash

# run docker-login --username <username> first

src_dir=$(dirname $0)
timestamp=$(date +"%Y%m%d-%H%M%S")
project_name="ipme-bitbucket"
instance_tag="${project_name}:${timestamp}"
repo_tag="hnaik2/${instance_tag}"

docker build -t ${instance_tag} ${src_dir}
docker tag ${instance_tag} ${repo_tag}
docker push ${repo_tag}
docker tag ${instance_tag} hnaik2/${project_name}:latest
docker push hnaik2/${project_name}:latest
