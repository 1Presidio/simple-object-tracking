Data Source: https://raw.githubusercontent.com/udacity/self-driving-car/master/annotations/labels_crowdai.csv


This is a simple module for a self driving car to do object tracking (however for our purposes is not live or on the fly).
The expected input is a simple csv or file of labeled data that includes the coordinates for the bounding boxes and the file 
which they refer to (typically the timestamp is the file name). My module will take in this csv file which is expected to follow a certain format and parse the data. Then, it is sorted numerically by time (based on the file name). Finally we proceed by 
following a file by file approach. This essentially means that we start with the first file, gather and analyse the objects given to us
and read the next file and analyse that file's contents. We then attempt to match up the objects use a naive centroid approach.
More specifically speaking we take the centroid of each bounding box and calculate the nearest neighbor. This is a naive and simple 
approach and most certainly is not robust for an actual self-driving car. Finally at the end of this pipeline the data is ready to
be passed on to the pipeline to be matched and paired. This can then be sent to the motion and planning control section of a car
where it can decide what to do with the data.

** Quickstart
1.) Make sure you have the right dependencies (MacOS or Linux). The code was built and run on a MacOS and should work on Linux 
or Unix based OSes.

2.) These are the dependencies:
    - Boost
    - C++11 or later
    - Bazel 0.9.0 or later

3.) 
    $ tar xvpf FILE_NAME.tar.gz && cd FILE_NAME
    $ wget https://raw.githubusercontent.com/udacity/self-driving-car/master/annotations/labels_crowdai.csv
    $ bazel build Perception
    $ bazel-bin/Perception labels_crowdai.csv # or where the binary is built to

You can view the results from stdout!
