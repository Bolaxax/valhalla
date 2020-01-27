#include "test.h"

#include <iostream>
#include <string>
#include <vector>

#include "baldr/rapidjson_utils.h"
#include "loki/worker.h"
#include "midgard/logging.h"
#include "sif/autocost.h"
#include "sif/bicyclecost.h"
#include "thor/isochrone.h"
#include "thor/worker.h"
#include <boost/property_tree/ptree.hpp>

using namespace valhalla;
using namespace valhalla::thor;
using namespace valhalla::sif;
using namespace valhalla::loki;
using namespace valhalla::baldr;
using namespace valhalla::midgard;
using namespace valhalla::tyr;

namespace {

boost::property_tree::ptree json_to_pt(const std::string& json) {
  std::stringstream ss;
  ss << json;
  boost::property_tree::ptree pt;
  rapidjson::read_json(ss, pt);
  return pt;
}

const auto config = json_to_pt(R"({
    "mjolnir":{"tile_dir":"test/data/utrecht_tiles", "concurrency": 1},
    "loki":{
      "actions":["sources_to_targets"],
      "logging":{"long_request": 100},
      "service_defaults":{"minimum_reachability": 50,"radius": 0,"search_cutoff": 35000, "node_snap_tolerance": 5, "street_side_tolerance": 5, "heading_tolerance": 60}
    },
    "thor":{
      "logging":{"long_request": 100}
    },
    "meili":{
      "grid": {
        "cache_size": 100240,
        "size": 500
      }
    },
    "service_limits": {
      "auto": {"max_distance": 5000000.0, "max_locations": 20,"max_matrix_distance": 400000.0,"max_matrix_locations": 50},
      "auto_shorter": {"max_distance": 5000000.0,"max_locations": 20,"max_matrix_distance": 400000.0,"max_matrix_locations": 50},
      "bicycle": {"max_distance": 500000.0,"max_locations": 50,"max_matrix_distance": 200000.0,"max_matrix_locations": 50},
      "bus": {"max_distance": 5000000.0,"max_locations": 50,"max_matrix_distance": 400000.0,"max_matrix_locations": 50},
      "hov": {"max_distance": 5000000.0,"max_locations": 20,"max_matrix_distance": 400000.0,"max_matrix_locations": 50},
      "taxi": {"max_distance": 5000000.0,"max_locations": 20,"max_matrix_distance": 400000.0,"max_matrix_locations": 50},
      "isochrone": {"max_contours": 4,"max_distance": 25000.0,"max_locations": 1,"max_time": 120},
      "max_avoid_locations": 50,"max_radius": 200,"max_reachability": 100,"max_alternates":2,
      "multimodal": {"max_distance": 500000.0,"max_locations": 50,"max_matrix_distance": 0.0,"max_matrix_locations": 0},
      "pedestrian": {"max_distance": 250000.0,"max_locations": 50,"max_matrix_distance": 200000.0,"max_matrix_locations": 50,"max_transit_walking_distance": 10000,"min_transit_walking_distance": 1},
      "skadi": {"max_shape": 750000,"min_resample": 10.0},
      "trace": {"max_distance": 200000.0,"max_gps_accuracy": 100.0,"max_search_radius": 100,"max_shape": 16000,"max_best_paths":4,"max_best_paths_shape":100},
      "transit": {"max_distance": 500000.0,"max_locations": 50,"max_matrix_distance": 200000.0,"max_matrix_locations": 50},
      "truck": {"max_distance": 5000000.0,"max_locations": 20,"max_matrix_distance": 400000.0,"max_matrix_locations": 50}
    }
  })");

} // namespace

void try_isochrone(GraphReader& reader,
                   loki_worker_t& loki_worker,
                   thor_worker_t& thor_worker,
                   const char* test_request,
                   const rapidjson::Document& expected) {
  Api request;
  ParseApi(test_request, Options::isochrone, request);
  loki_worker.isochrones(request);

  // Process isochrone request
  auto response_json = thor_worker.isochrones(request);

  rapidjson::Document response;
  response.Parse(response_json);

  // Check if the result contains the expected string
  if (response != expected) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    expected.Accept(writer);
    const std::string expected_str = buffer.GetString();

    throw std::runtime_error("isochrones failed:\nexpected\n" + expected_str + "\nreceived\n" +
                             response_json + "\n");
  }
}

void test_isochrones() {
  // Test setup
  loki_worker_t loki_worker(config);
  thor_worker_t thor_worker(config);
  GraphReader reader(config.get_child("mjolnir"));

// Test auto isochrone with one contour
// 32bit builds fail in release mode we'll look at this separately
#if _WIN64 || __amd64__
  {
    std::cout << "test-case 1" << std::endl;

    rapidjson::Document expected1;
    expected1.Parse(
        R"({"features":[{"properties":{"fillOpacity":0.33,"fill-opacity":0.33,"opacity":0.33,"fillColor":"#bf4040","fill":"#bf4040","color":"#bf4040","contour":15},"geometry":{"coordinates":[[5.057326,52.133942],[5.042322,52.128365],[5.036321,52.122482],[5.033277,52.122978],[5.033321,52.118866],[5.016042,52.128220],[5.029585,52.119202],[5.027321,52.110050],[5.034322,52.116291],[5.051321,52.110027],[5.050166,52.106781],[5.060853,52.101467],[5.055890,52.098938],[5.058862,52.098480],[5.060247,52.094864],[5.062593,52.096210],[5.062321,52.090961],[5.065204,52.090939],[5.063321,52.088036],[5.048326,52.085938],[5.042562,52.072697],[5.035111,52.073727],[5.037410,52.074936],[5.034438,52.078938],[5.021885,52.075375],[5.026835,52.069939],[5.023302,52.068962],[5.017321,52.060989],[5.031321,52.071373],[5.051322,52.067150],[5.054321,52.068146],[5.057321,52.065022],[5.059321,52.067131],[5.063456,52.066071],[5.064321,52.056660],[5.065321,52.058754],[5.068246,52.057014],[5.069321,52.062141],[5.084446,52.061062],[5.083321,52.057690],[5.077094,52.057713],[5.078321,52.060291],[5.069109,52.059151],[5.068875,52.056938],[5.070321,52.058552],[5.073695,52.057564],[5.071198,52.057060],[5.069729,52.051529],[5.066163,52.050938],[5.072966,52.046585],[5.093321,52.046242],[5.094072,52.048187],[5.098321,52.046185],[5.100210,52.050045],[5.107513,52.051128],[5.111443,52.050056],[5.111743,52.046360],[5.114144,52.049118],[5.114837,52.046455],[5.118321,52.046101],[5.120321,52.049103],[5.141321,52.050182],[5.142321,52.048122],[5.145168,52.052090],[5.155321,52.051189],[5.159081,52.054939],[5.155189,52.057076],[5.159908,52.059525],[5.152190,52.058804],[5.149157,52.065933],[5.152321,52.068085],[5.158982,52.064278],[5.159007,52.066624],[5.152083,52.070938],[5.161846,52.079414],[5.163102,52.091938],[5.153668,52.102940],[5.161827,52.104939],[5.154067,52.104683],[5.146321,52.122192],[5.144503,52.119938],[5.152765,52.104492],[5.151073,52.103691],[5.145321,52.110264],[5.139665,52.105934],[5.142750,52.105366],[5.142321,52.103615],[5.136321,52.106106],[5.132321,52.103809],[5.132320,52.107155],[5.129321,52.104534],[5.119449,52.105938],[5.129852,52.102940],[5.128848,52.098412],[5.126446,52.098816],[5.121603,52.092937],[5.124551,52.090710],[5.120321,52.089500],[5.112284,52.092941],[5.117544,52.093716],[5.119348,52.099911],[5.116074,52.099686],[5.114321,52.104713],[5.112321,52.101658],[5.105321,52.104347],[5.104146,52.100765],[5.103251,52.102867],[5.094388,52.104939],[5.094551,52.107170],[5.088172,52.105091],[5.085321,52.101357],[5.081321,52.103901],[5.084321,52.099812],[5.070321,52.099251],[5.055766,52.110943],[5.059321,52.112495],[5.063899,52.108936],[5.057714,52.116940],[5.060321,52.119476],[5.051800,52.117939],[5.053321,52.116348],[5.047529,52.116940],[5.055627,52.115242],[5.053321,52.111626],[5.041321,52.115639],[5.036321,52.119511],[5.045849,52.117939],[5.042403,52.123020],[5.041321,52.121441],[5.039000,52.123260],[5.048522,52.130737],[5.057326,52.133942]],"type":"LineString"},"type":"Feature"}],"type":"FeatureCollection"})");
    const auto test_request1 =
        R"({"locations":[{"lat":52.078937,"lon":5.115321}],"costing":"auto","contours":[{"time":15}],"polygons":false,"denoise":0.2,"generalize":150})";
    try_isochrone(reader, loki_worker, thor_worker, test_request1, expected1);
  }

  {
    std::cout << "Try pedestrian isochrone with one contour, polygon=true" << std::endl;

    rapidjson::Document expected;
    expected.Parse(
        R"({"features":[{"properties":{"fillOpacity":0.33,"fill-opacity":0.33,"opacity":0.33,"fillColor":"#bf4040","fill":"#bf4040","color":"#bf4040","contour":15},"geometry":{"coordinates":[[[5.116750,52.105366],[5.116321,52.105911],[5.115321,52.104435],[5.114203,52.105053],[5.112321,52.104988],[5.111320,52.104378],[5.110528,52.105148],[5.109321,52.105453],[5.108586,52.104939],[5.109802,52.104420],[5.109958,52.103939],[5.108322,52.102592],[5.108109,52.104725],[5.106321,52.105621],[5.105320,52.105598],[5.104321,52.104500],[5.099321,52.104610],[5.098321,52.103855],[5.097321,52.104488],[5.096321,52.104465],[5.094315,52.103943],[5.094600,52.102940],[5.093812,52.101940],[5.094321,52.101170],[5.094631,52.101631],[5.095745,52.101364],[5.098321,52.098408],[5.100321,52.098270],[5.101322,52.097263],[5.103321,52.097271],[5.103322,52.095802],[5.103026,52.096645],[5.102321,52.096790],[5.095321,52.096752],[5.094321,52.097729],[5.088070,52.097691],[5.087966,52.098293],[5.089468,52.098789],[5.089511,52.099751],[5.090385,52.099937],[5.088321,52.100094],[5.087321,52.101532],[5.085804,52.100941],[5.085321,52.099815],[5.082623,52.098938],[5.085321,52.098572],[5.085574,52.097687],[5.081321,52.097294],[5.080321,52.097778],[5.080232,52.096851],[5.082321,52.095627],[5.082532,52.094944],[5.081321,52.094818],[5.080321,52.095531],[5.078957,52.095303],[5.079924,52.093540],[5.081647,52.092934],[5.078921,52.092335],[5.078769,52.090939],[5.079899,52.088516],[5.081713,52.087936],[5.081321,52.087139],[5.080268,52.086994],[5.080030,52.083939],[5.081628,52.082249],[5.081663,52.079941],[5.080321,52.079357],[5.078321,52.079407],[5.077321,52.080475],[5.076321,52.080681],[5.076146,52.079941],[5.077321,52.078571],[5.082321,52.078285],[5.082602,52.076664],[5.081840,52.075939],[5.081925,52.074944],[5.084600,52.071655],[5.082191,52.071804],[5.081908,52.072525],[5.079140,52.074760],[5.079321,52.075817],[5.080047,52.075935],[5.078625,52.077244],[5.077321,52.077320],[5.075321,52.075466],[5.070601,52.074940],[5.071321,52.074463],[5.074321,52.074436],[5.074943,52.073563],[5.076543,52.073162],[5.076563,52.071693],[5.075321,52.071480],[5.074780,52.070938],[5.074975,52.069592],[5.075935,52.068939],[5.075899,52.065517],[5.081321,52.065224],[5.084018,52.062637],[5.086631,52.062244],[5.087321,52.061298],[5.089321,52.060841],[5.089705,52.061939],[5.088805,52.062939],[5.089091,52.064171],[5.090559,52.064178],[5.090906,52.063522],[5.092623,52.063244],[5.093321,52.062454],[5.094321,52.064041],[5.094564,52.063179],[5.095458,52.063076],[5.095690,52.062309],[5.096459,52.062077],[5.097842,52.060463],[5.100718,52.058334],[5.101322,52.056812],[5.102361,52.056900],[5.102895,52.057365],[5.104321,52.056190],[5.106467,52.056087],[5.106321,52.055298],[5.105321,52.054512],[5.104321,52.054993],[5.103530,52.054726],[5.103488,52.053940],[5.102596,52.053669],[5.102394,52.052864],[5.100932,52.052334],[5.100321,52.051456],[5.096584,52.051201],[5.096321,52.051891],[5.096031,52.051228],[5.096971,52.050587],[5.102708,52.050327],[5.104321,52.048225],[5.104509,52.050129],[5.103671,52.051941],[5.104067,52.052193],[5.104522,52.052143],[5.104743,52.051361],[5.107697,52.051315],[5.108322,52.050476],[5.111721,52.050339],[5.112321,52.048714],[5.112767,52.049496],[5.114082,52.049934],[5.115120,52.050941],[5.113321,52.052467],[5.112790,52.051472],[5.111043,52.051659],[5.112013,52.052940],[5.112134,52.055130],[5.114321,52.055340],[5.115321,52.056591],[5.118145,52.056763],[5.118321,52.056465],[5.118402,52.056938],[5.119127,52.057133],[5.121322,52.057213],[5.123321,52.059143],[5.125321,52.059208],[5.126321,52.058254],[5.127515,52.059135],[5.128184,52.056801],[5.131782,52.056400],[5.132321,52.055725],[5.133321,52.056396],[5.139321,52.056801],[5.139943,52.058319],[5.141661,52.059601],[5.141804,52.060944],[5.143855,52.062405],[5.144321,52.064144],[5.144796,52.062416],[5.149988,52.057602],[5.151731,52.057350],[5.152321,52.055592],[5.152554,52.058170],[5.150945,52.058563],[5.148050,52.061943],[5.148321,52.062473],[5.149321,52.062180],[5.149666,52.062595],[5.148878,52.063938],[5.148989,52.065941],[5.151096,52.068165],[5.152321,52.068291],[5.153321,52.067326],[5.154643,52.067261],[5.156321,52.065590],[5.158297,52.065941],[5.156879,52.066502],[5.155656,52.068275],[5.154321,52.068508],[5.153075,52.069691],[5.153321,52.070408],[5.155321,52.070553],[5.155579,52.071194],[5.152321,52.071537],[5.151321,52.070644],[5.149110,52.072945],[5.150073,52.073940],[5.150126,52.075138],[5.151321,52.076324],[5.152967,52.074940],[5.153321,52.073830],[5.153959,52.075306],[5.155551,52.075710],[5.155489,52.078106],[5.153321,52.076469],[5.151932,52.077938],[5.153321,52.079437],[5.154593,52.079666],[5.155321,52.080685],[5.156143,52.080765],[5.156321,52.080048],[5.157012,52.082939],[5.155893,52.083363],[5.153646,52.081615],[5.152322,52.081535],[5.151571,52.080692],[5.150321,52.080620],[5.149127,52.079742],[5.149169,52.081097],[5.151133,52.083130],[5.153790,52.083469],[5.153936,52.086327],[5.154797,52.087467],[5.156342,52.087959],[5.154321,52.089752],[5.153321,52.089428],[5.150321,52.089512],[5.149321,52.086674],[5.149110,52.091152],[5.152321,52.091389],[5.154321,52.090439],[5.154446,52.090939],[5.153837,52.091454],[5.153606,52.093220],[5.152321,52.094185],[5.151321,52.094181],[5.150321,52.092571],[5.149321,52.093456],[5.148564,52.092693],[5.147577,52.092937],[5.147996,52.094269],[5.149571,52.094688],[5.149889,52.095371],[5.151320,52.095100],[5.152143,52.095940],[5.151929,52.097332],[5.154625,52.097939],[5.153614,52.098228],[5.153321,52.099056],[5.152812,52.098446],[5.151321,52.098381],[5.150321,52.099773],[5.146321,52.099003],[5.145781,52.099937],[5.146883,52.101940],[5.144915,52.101345],[5.144862,52.099937],[5.144321,52.099510],[5.142867,52.100941],[5.143321,52.101604],[5.144900,52.101940],[5.142525,52.103142],[5.139321,52.102150],[5.136211,52.103050],[5.135880,52.102379],[5.134145,52.102119],[5.133655,52.100605],[5.131934,52.100323],[5.130784,52.099476],[5.130188,52.097805],[5.128321,52.098148],[5.126321,52.099487],[5.124321,52.098881],[5.123845,52.099937],[5.124456,52.101074],[5.124337,52.102940],[5.122321,52.103409],[5.120321,52.103371],[5.119321,52.102829],[5.118321,52.104259],[5.116750,52.105366]]],"type":"Polygon"},"type":"Feature"}],"type":"FeatureCollection"})");
    const auto test_request2 =
        R"({"locations":[{"lat":52.078937,"lon":5.115321}],"costing":"bicycle","contours":[{"time":15}],"polygons":true,"denoise":0.2})";
    try_isochrone(reader, loki_worker, thor_worker, test_request2, expected);
  }
#endif
}

int main(int argc, char* argv[]) {
  test::suite suite("isochrones");

  // Silence logs (especially long request logging)
  logging::Configure({{"type", ""}});

  suite.test(TEST_CASE(test_isochrones));

  return suite.tear_down();
}
