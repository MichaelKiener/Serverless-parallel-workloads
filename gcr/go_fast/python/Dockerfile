FROM python:3.8-slim

# Allow statements and log messages to immediately appear in the Knative logs
ENV PYTHONUNBUFFERED True
# WORKDIR $APP_HOME

# Copy local code to the container image.

# Copy function code
COPY main.py requirements.txt ./

# Install the function's dependencies using file requirements.txt
# from your project folder.

RUN python3.8 -m pip install --no-cache-dir -r requirements.txt
RUN pip show tbb
ENV LD_LIBRARY_PATH=/usr/local/lib/python3.8:/usr/local/lib/python3.8/site-packages:usr/local/lib:/var/lang/lib:$LD_LIBRARY_PATH
RUN python3.8 -m pip install Flask gunicorn

# Set the CMD to your handler (could also be done as a parameter override outside of the Dockerfile)
CMD exec gunicorn --bind :$PORT --workers 1 --threads 6 --timeout 0 main:app
