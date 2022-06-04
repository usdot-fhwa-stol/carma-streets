#include "map_msg_worker.h"
#include <vector>
#include <iostream>

namespace intersection_model
{
    map_msg_worker::map_msg_worker(const int intersection_id, const std::string &bootstrap_server, const std::string &map_msg_group_id, const std::string &map_msg_topic_name)
    {
        this->bootstrap_server = bootstrap_server;
        this->map_msg_group_id = map_msg_group_id;
        this->map_msg_topic_name = map_msg_topic_name;
        /**
         * Kafka client config
         */
        auto client = std::make_shared<kafka_clients::kafka_client>();
        _map_msg_consumer = client->create_consumer(this->bootstrap_server, this->map_msg_topic_name, this->map_msg_group_id);
        if (!_map_msg_consumer || !_map_msg_consumer->init())
        {
            SPDLOG_CRITICAL("kafka consumers (_map_msg_consumer) initialize error");
            exit(-1);
        }
        else
        {
            _map_msg_consumer->subscribe();
            if (!_map_msg_consumer->is_running())
            {
                SPDLOG_CRITICAL("consumer_workers (_map_msg_consumer) is not running");
                exit(-1);
            }
        }
        _intersection_map_ptr = std::make_shared<intersection_map>();
        _intersection_map_ptr->intersectionid = intersection_id;
        SPDLOG_INFO("map_msg_worker constructor done");
    }

    map_msg_worker::map_msg_worker(const int intersection_id)
    {
        _intersection_map_ptr = std::make_shared<intersection_map>();
        _intersection_map_ptr->intersectionid = intersection_id;
    }

    map_msg_worker::~map_msg_worker()
    {
        if (_map_msg_consumer)
        {
            _map_msg_consumer->stop();
        }
        SPDLOG_INFO("~map_msg_worker destructed");
    }

    bool map_msg_worker::update_map_msg(const std::string &payload)
    {
        bool is_updated = false;
        std::unique_lock<std::mutex> lck(intersection_map_msg_mtx);
        if (_intersection_map_ptr == nullptr)
        {
            SPDLOG_CRITICAL("Intersection map msg is nullptr");
            return is_updated;
        }
        rapidjson::Document doc = parse_JSON_string(payload);
        // Map data
        if (!doc.HasParseError() && doc.FindMember("map_data")->value.IsObject())
        {
            auto mapData = doc.FindMember("map_data")->value.GetObject();
            // Intersections
            if (mapData.FindMember("intersections")->value.IsObject())
            {
                auto intersections = mapData.FindMember("intersections")->value.GetObject();
                // Intersection geometry
                if (intersections.FindMember("intersection_geometry")->value.IsObject())
                {
                    auto intersection_geometry = intersections.FindMember("intersection_geometry")->value.GetObject();
                    auto id = intersection_geometry.FindMember("id")->value.GetObject();
                    auto id_id = std::stoul(id.FindMember("id")->value.GetString()); // id is getting from kafka consumer
                    bool is_intersection_id_match = (id_id == _intersection_map_ptr->intersectionid) ? true : false; //intersectionid is initialized value from manifest.json
                    if (is_intersection_id_match) //Make sure the intersection id in the manifest.json match the id from the MAP message
                    {
                        SPDLOG_DEBUG("Find mapping intersection ID.");
                        map_geometry mapGeo;
                        if (intersection_geometry.FindMember("ref_point")->value.IsObject())
                        {
                            auto ref_point = intersection_geometry.FindMember("ref_point")->value.GetObject();
                            mapGeo.refpoint.latitude = std::stol(ref_point.FindMember("lat")->value.GetString());
                            mapGeo.refpoint.longitude = std::stol(ref_point.FindMember("long")->value.GetString());
                            mapGeo.refpoint.elevation = std::stol(ref_point.FindMember("elevation")->value.GetString());
                        }
                        mapGeo.approach.width = std::stol(intersection_geometry.FindMember("lane_width")->value.GetString());

                        // Lane set
                        if (intersection_geometry.FindMember("lane_set")->value.IsArray())
                        {
                            auto lane_set = intersection_geometry.FindMember("lane_set")->value.GetArray();
                            for (auto j = 0; j < lane_set.Size(); j++)
                            {
                                map_lane lane;
                                lane.lane_id = std::stol(lane_set[j].FindMember("lane_id")->value.GetString());

                                // Node list
                                if (lane_set[j].FindMember("node_list")->value.FindMember("nodes")->value.IsArray())
                                {
                                    auto nodes = lane_set[j].FindMember("node_list")->value.FindMember("nodes")->value.GetArray();
                                    for (auto k = 0; k < nodes.Size(); k++)
                                    {
                                        map_node map_node;
                                        map_node.x = std::stol(nodes[k].FindMember("delta")->value.FindMember("node-xy")->value.FindMember("x")->value.GetString());
                                        map_node.y = std::stol(nodes[k].FindMember("delta")->value.FindMember("node-xy")->value.FindMember("y")->value.GetString());
                                        lane.nodes.push_back(map_node);
                                    }
                                }

                                if (lane_set[j].FindMember("connects_to")->value.IsArray())
                                {
                                    // Lane connections with signal group ids
                                    auto connectsto = lane_set[j].FindMember("connects_to")->value.GetArray();
                                    for (auto k = 0; k < connectsto.Size(); k++)
                                    {
                                        map_connection map_connect;
                                        map_connect.lane_id = std::stol(connectsto[k].FindMember("connecting_lane")->value.FindMember("lane")->value.GetString());
                                        map_connect.signalGroup = std::stol(connectsto[k].FindMember("signal_group")->value.GetString());
                                        lane.connection.push_back(map_connect);
                                    }
                                }
                                mapGeo.approach.lanes.push_back(lane);
                            }
                            SPDLOG_DEBUG("Updated lane set.");
                        }
                        _intersection_map_ptr->geometries.clear();
                        _intersection_map_ptr->geometries.push_back(mapGeo);
                        SPDLOG_DEBUG("Update map msg done.");
                        is_updated = true;
                    }
                    else
                    {
                        SPDLOG_CRITICAL("Cannot find intersection model intersection_id mapping to MAP msg intersection geometry id.");
                    }
                }
            }
        }
        return is_updated;
    }

    const std::shared_ptr<intersection_map> map_msg_worker::get_map_msg_ptr() const
    {
        return _intersection_map_ptr;
    }

    const std::shared_ptr<kafka_clients::kafka_consumer_worker> map_msg_worker::get_map_msg_consumer_ptr() const
    {
        return _map_msg_consumer;
    }

    rapidjson::Document map_msg_worker::parse_JSON_string(const std::string &jsonString)
    {
        rapidjson::Document doc;
        bool has_parse_error = doc.Parse(jsonString.c_str()).HasParseError() ? true : false;

        if (has_parse_error)
        {
            SPDLOG_CRITICAL("Document parse error: Cannot parse map JSON string");
        }
        return doc;
    }

}