zone := europe-north1-a
project_id := parcs-lab-455419

leader_url_file = leader_url.txt
image ?= hayashi97m/parcs-lab-runner-go:latest
extra_env ?=

WORKERS ?= 3
MAX_PRIME ?= 100000

delete-all:
	gcloud compute instances delete leader $(shell seq -f "worker-%g" 1 $(WORKERS)) --zone $(zone) --project $(project_id)
	gcloud compute firewall-rules delete swarmpit

create-all:
	./gce-parcs.sh WORKERS=$(WORKERS)

run-image:
	gcloud compute ssh leader --command "sudo docker service create --network parcs --restart-condition none --env LEADER_URL=$(shell cat $(leader_url_file)) --env WORKERS=$(WORKERS) --env N=$(MAX_PRIME) $(extra_env) -d $(image)"

run-lab:
	make run-image image=hayashi97m/parcs-lab-runner-go:latest extra_env="--env WORKERS=$(WORKERS) --env N=$(MAX_PRIME)"

build-and-push-runner:
	docker build -t hayashi97m/parcs-lab-runner-go:latest ./runner
	docker push hayashi97m/parcs-lab-runner-go:latest

build-and-push-worker:
	docker build -t hayashi97m/parcs-lab-worker-go:latest ./worker
	docker push hayashi97m/parcs-lab-worker-go:latest
