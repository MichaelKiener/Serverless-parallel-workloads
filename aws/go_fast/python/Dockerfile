FROM public.ecr.aws/lambda/python:3.8

# Copy function code
COPY go_fast.py requirements.txt ./

# Install the function's dependencies using file requirements.txt
# from your project folder.

RUN python3.8 -m pip install --no-cache-dir -r requirements.txt
ENV LD_LIBRARY_PATH=/var/lang/lib:$LD_LIBRARY_PATH


# Set the CMD to your handler (could also be done as a parameter override outside of the Dockerfile)
CMD [ "go_fast.lambda_handler" ]
