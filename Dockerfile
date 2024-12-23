FROM public.ecr.aws/lambda/nodejs:latest

RUN microdnf install -y git bison flex automake gcc make glib2-static.x86_64

