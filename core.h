#include <boost/geometry.hpp>
#include <boost/tokenizer.hpp>
#include <map>
#include <string>

typedef std::vector<
    boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>>
    BoundingBox;

typedef boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>
    BoostPoint;

typedef boost::tokenizer<boost::escaped_list_separator<char>> tokenizer;

typedef std::vector<std::string> StringPts;

// class definition
class ObjectTracker {
 public:
  ObjectTracker();

  ~ObjectTracker();

  std::map<int, std::map<std::string, StringPts>> trackers;

  void readCSVFile(const std::string &labels,
                   std::map<std::string, std::vector<StringPts>> *csvData);

  void trackFrames(std::map<std::string, std::vector<StringPts>> csvData,
                   std::map<int, std::map<std::string, StringPts>> *trackers);

  void matchTrackers(std::vector<StringPts> prevObj,
                     std::vector<StringPts> currObj,
                     std::map<int, std::map<std::string, StringPts>> *trackers);

  std::vector<StringPts> getPreviousObjects(
      const std::string &timestamp,
      const std::map<std::string, std::vector<StringPts>> &csvData);

  static void convertToBoundingBox(const std::vector<std::string> stringPts,
                                   BoundingBox *box);
};
