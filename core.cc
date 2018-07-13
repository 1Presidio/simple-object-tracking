#include "core.h"
#include <assert.h>
#include <boost/geometry.hpp>
#include <boost/geometry/core/point_type.hpp>
#include <boost/tokenizer.hpp>
#include <fstream>  // fstream
#include <iostream>
#include <map>
#include <string>
#include <vector>

// constructor
ObjectTracker::ObjectTracker() {}

// destructor
ObjectTracker::~ObjectTracker() {}

// converts strings to a boost compatible point type
void ObjectTracker::convertToBoundingBox(const StringPts stringPts,
                                         BoundingBox *box) {
  // as the order the csv was read in we have xmin,ymin,xmax,ymax
  // |c d|
  // |a b|
  BoostPoint a(stod(stringPts[0]), stod(stringPts[1]));
  BoostPoint b(stod(stringPts[2]), stod(stringPts[1]));
  BoostPoint c(stod(stringPts[0]), stod(stringPts[3]));
  BoostPoint d(stod(stringPts[2]), stod(stringPts[3]));
  (*box).push_back(a);
  (*box).push_back(b);
  (*box).push_back(c);
  (*box).push_back(d);
}

// reads a CSV file into csvData
void ObjectTracker::readCSVFile(
    const std::string &labels,
    std::map<std::string, std::vector<StringPts>> *csvData) {
  std::ifstream ifs(labels.c_str());
  if (!ifs.is_open()) {
    std::cout << "Failed to open ifstream.. Exiting\n";
    exit(1);
  }

  bool isFirst = true;  // skip first line since it is a header
  StringPts dataStream;
  std::string readLine;
  while (getline(ifs, readLine)) {
    if (isFirst) {
      // skip headers
      isFirst = false;
      continue;
    }
    tokenizer token(readLine);
    dataStream.assign(token.begin(), token.end());
    std::string fileName;
    std::string objectType;
    StringPts stringPts;
    for (size_t i = 0; i < dataStream.size(); ++i) {
      if (i < 4) {
        // first 4 numbers are box dimensions
        stringPts.push_back(dataStream[i]);
      } else if (i == 4) {
        fileName = dataStream[i];
      }  // ignore url seems to be broken
    }
    if (csvData->find(fileName) != csvData->end()) {
      (*csvData)[fileName].push_back(stringPts);
    } else {
      std::vector<StringPts> ptsVec;
      ptsVec.push_back(stringPts);
      csvData->emplace(fileName, ptsVec);
    }
  }
}

// gets the previous timestamp of the object we are going to compare
std::vector<StringPts> ObjectTracker::getPreviousObjects(
    const std::string &timestamp,
    const std::map<std::string, std::vector<StringPts>> &csvData) {
  std::vector<StringPts> res;
  auto it = csvData.begin();
  while (it != csvData.end()) {
    res = it->second;
    ++it;
    if (it->first == timestamp) {
      return res;
    }
  }
  return csvData.find(timestamp)->second;
}

// calculates distance between centroids
double calculateDistance(const StringPts &p1, const StringPts &p2) {
  BoundingBox box1, box2;
  ObjectTracker::convertToBoundingBox(p1, &box1);
  ObjectTracker::convertToBoundingBox(p2, &box2);
  BoostPoint centre1, centre2;
  boost::geometry::model::ring<BoostPoint> ring1, ring2;
  assert(p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); ++i) {
    boost::geometry::append(ring1, box1[i]);
    boost::geometry::append(ring2, box2[i]);
  }
  boost::geometry::centroid(ring1, centre1);
  boost::geometry::centroid(ring2, centre2);
  return static_cast<double>(boost::geometry::distance(centre1, centre2));
}

// attempt a simple match the match found is in assignments and can be passed on
// to the planning module
void ObjectTracker::matchTrackers(
    std::vector<StringPts> prevObj, std::vector<StringPts> currObj,
    std::map<int, std::map<std::string, StringPts>> *trackers) {
  std::vector<int> assignments(currObj.size(), -1);
  for (size_t i = 0; i < currObj.size(); ++i) {
    int minDist = 1000;
    int minIndex = -1;
    for (size_t j = 0; j < prevObj.size(); ++j) {
      double dist = calculateDistance(currObj[i], prevObj[j]);
      if (dist < minDist) {
        minIndex = j;
        minDist = dist;
      }
    }
    assignments[i] = minIndex;
    // length in pixels
    if (minIndex != -1) {
      std::cout << "minDistance Found was: "
                << calculateDistance(currObj[i], prevObj[minIndex])
                << std::endl;
    }
  }
}

// attempt to track frame by frame for the car (i.e file by file)
void ObjectTracker::trackFrames(
    std::map<std::string, std::vector<StringPts>> csvData,
    std::map<int, std::map<std::string, StringPts>> *trackers) {
  bool isFirst = true;
  int trackId = 1;
  for (auto fileIt : csvData) {
    if (isFirst) {
      isFirst = false;
      for (auto i = 0; i < fileIt.second.size(); ++i) {
        (*trackers)[trackId].emplace(fileIt.first, fileIt.second[i]);
        ++trackId;
      }
    } else {
      std::string timestamp = fileIt.first;
      std::vector<StringPts> prevObj = getPreviousObjects(timestamp, csvData);
      matchTrackers(prevObj, fileIt.second, trackers);
    }
  }
  return;
}

int main(int argc, char **argv) {
  // expects one argument that is the path to the csv for labeled data
  if (argc != 2) {
    std::cerr << "Bad Args\n";
    return 1;
  }

  ObjectTracker objt_ = ObjectTracker();  // create instance of object
  std::string labels(argv[1]);
  std::map<std::string, std::vector<StringPts>> csvData;
  objt_.readCSVFile(labels, &csvData);
  objt_.trackFrames(csvData, &objt_.trackers);
  std::cout << "Done\n";
  return 0;
}