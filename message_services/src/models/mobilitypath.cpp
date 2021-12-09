#include <iostream>
#include <string>
#include <boost/algorithm/string.hpp>

#include "mobilitypath.h"

namespace message_services
{

    namespace models
    {

        mobilitypath::mobilitypath() : header(), trajectory() {}

        mobilitypath::~mobilitypath() {}

        void mobilitypath::fromJsonObject(const rapidjson::Value &obj)
        {
            if (obj.IsObject())
            {
                for (rapidjson::Value::ConstMemberIterator obj_itr = obj.MemberBegin(); obj_itr != obj.MemberEnd(); obj_itr++)
                {
                    if (obj_itr->value.IsObject() && boost::iequals(std::string(obj_itr->name.GetString()), std::string("metadata")))
                    {
                        mobility_header_t header;

                        for (rapidjson::Value::ConstMemberIterator header_itr = obj_itr->value.MemberBegin(); header_itr != obj_itr->value.MemberEnd(); header_itr++)
                        {
                            if (std::string(header_itr->name.GetString()) == std::string("hostStaticId"))
                            {
                                header.sender_id = header_itr->value.GetString();
                            }

                            if (std::string(header_itr->name.GetString()) == std::string("targetStaticId"))
                            {
                                header.recipient_id = (header_itr->value.GetString());
                            }

                            if (std::string(header_itr->name.GetString()) == std::string("hostBSMId"))
                            {
                                header.sender_bsm_id = (header_itr->value.GetString());
                            }

                            if (std::string(header_itr->name.GetString()) == std::string("planId"))
                            {
                                header.plan_id = (header_itr->value.GetString());
                            }
                            if (std::string(header_itr->name.GetString()) == std::string("timestamp"))
                            {
                                header.timestamp = std::stol(header_itr->value.GetString());
                            }
                        }
                        setHeader(header);
                    }

                    if (obj_itr->value.IsObject() && boost::iequals(std::string(obj_itr->name.GetString()), std::string("trajectory")))
                    {
                        // trajectory_t trajectory;

                        for (rapidjson::Value::ConstMemberIterator tr_itr = obj_itr->value.MemberBegin(); tr_itr != obj_itr->value.MemberEnd(); tr_itr++)
                        {
                            if (tr_itr->value.IsObject() && boost::iequals(std::string(tr_itr->name.GetString()), std::string("location")))
                            {
                                for (rapidjson::Value::ConstMemberIterator loc_itr = tr_itr->value.MemberBegin(); loc_itr != tr_itr->value.MemberEnd(); loc_itr++)
                                {
                                    if (std::string(loc_itr->name.GetString()) == std::string("ecefX"))
                                    {
                                        this->trajectory.location.ecef_x = (loc_itr->value.GetInt64());
                                    }

                                    if (std::string(loc_itr->name.GetString()) == std::string("ecefY"))
                                    {
                                        this->trajectory.location.ecef_y = (loc_itr->value.GetInt64());
                                    }

                                    if (std::string(loc_itr->name.GetString()) == std::string("ecefZ"))
                                    {
                                        this->trajectory.location.ecef_z = (loc_itr->value.GetInt64());
                                    }

                                    if (std::string(loc_itr->name.GetString()) == std::string("timestamp"))
                                    {
                                        this->trajectory.location.timestamp = std::stol(loc_itr->value.GetString());
                                    }
                                }
                            }
                            if (boost::iequals(std::string(tr_itr->name.GetString()), std::string("offsets")) && tr_itr->value.IsArray())
                            {
                                for (rapidjson::Value::ConstValueIterator offset_itr = tr_itr->value.Begin(); offset_itr != tr_itr->value.End(); offset_itr++)
                                {
                                    models::locationOffsetECEF offset;
                                    const rapidjson::Value &offset_value = *offset_itr;
                                    for (rapidjson::Value::ConstMemberIterator each_offset_itr = offset_value.MemberBegin(); each_offset_itr != offset_value.MemberEnd(); each_offset_itr++)
                                    {
                                        if (std::string(each_offset_itr->name.GetString()) == std::string("offsetX"))
                                        {
                                            offset.offset_x = (each_offset_itr->value.GetInt());
                                        }

                                        if (std::string(each_offset_itr->name.GetString()) == std::string("offsetY"))
                                        {
                                            offset.offset_y = (each_offset_itr->value.GetInt());
                                        }

                                        if (std::string(each_offset_itr->name.GetString()) == std::string("offsetZ"))
                                        {
                                            offset.offset_z = (each_offset_itr->value.GetInt());
                                        }
                                    }
                                    this->trajectory.offsets.push_back(offset);
                                }
                            }
                        }
                    }
                }
            }
        }

        bool mobilitypath::asJsonObject(rapidjson::Writer<rapidjson::StringBuffer> *writer) const
        {
            try
            {
                writer->StartObject();
                writer->Key("header");
                writer->StartObject();
                writer->Key("sender_id");
                writer->String(this->getHeader().sender_id.c_str());
                writer->Key("recipient_id");
                writer->String(this->getHeader().recipient_id.c_str());
                writer->Key("plan_id");
                writer->String(this->getHeader().plan_id.c_str());
                writer->Key("timestamp");
                writer->Uint64(this->getHeader().timestamp);
                writer->EndObject();
                writer->Key("trajectory");
                writer->StartObject();
                writer->Key("location");
                writer->StartObject();
                writer->Key("ecef_x");
                writer->Uint64(this->getTrajectory().location.ecef_x);
                writer->Key("ecef_y");
                writer->Uint64(this->getTrajectory().location.ecef_y);
                writer->Key("ecef_z");
                writer->Uint64(this->getTrajectory().location.ecef_z);
                writer->Key("timestamp");
                writer->Uint64(this->getTrajectory().location.timestamp);
                writer->EndObject();
                writer->Key("offsets");
                for (int i = 0; i < this->getTrajectory().MAX_POINTS_IN_MESSAGE; i++)
                {
                    writer->StartObject();
                    writer->Key("offset_x");
                    writer->Uint64(this->getTrajectory().offsets[i].offset_z);
                    writer->Key("offset_y");
                    writer->Uint64(this->getTrajectory().offsets[i].offset_z);
                    writer->Key("offset_z");
                    writer->Uint64(this->getTrajectory().offsets[i].offset_z);
                    writer->EndObject();
                }

                writer->EndObject();
                writer->EndObject();

                return true;
            }
            catch (std::exception &ex)
            {
                spdlog::critical("mobilitypath model: ", ex.what());
                return false;
            }
        }

        std::ostream &operator<<(std::ostream &out, mobilitypath &mobilitypath_obj)
        {
            std::cout << std::fixed;
            std::cout << std::setprecision(6);
            std::cout << "mobilitypath model: sender_id =  " << mobilitypath_obj.header.sender_id
                      << ", sender_bsm_id =" << mobilitypath_obj.header.sender_bsm_id
                      << ", timestamp = " << mobilitypath_obj.header.timestamp
                      << ", location = { ecef_x= " << mobilitypath_obj.trajectory.location.ecef_x << ", ecef_y= "
                      << mobilitypath_obj.trajectory.location.ecef_y << ", ecef_z= " << mobilitypath_obj.trajectory.offsets[0].offset_x << ", offsets = { offset_x= " << mobilitypath_obj.trajectory.location.ecef_x << ", ecef_y= "
                      << mobilitypath_obj.trajectory.offsets[0].offset_y << ", offset_z= " << mobilitypath_obj.trajectory.offsets[0].offset_z << "}" << std::endl;
        }

        std::string mobilitypath::generate_hash_sender_timestamp_id(std::string sender_bsm_id, uint64_t timestamp)
        {
            std::stringstream hash_ss;
            hash_ss << sender_bsm_id << timestamp;
            return hash_ss.str();
        }
        mobility_header_t mobilitypath::getHeader() const
        {
            return this->header;
        }
        void mobilitypath::setHeader(mobility_header_t header)
        {
            this->header = header;
        }

        trajectory_t mobilitypath::getTrajectory() const
        {
            return this->trajectory;
        }
        void mobilitypath::setTrajectory(trajectory_t trajectory)
        {
            this->trajectory = trajectory;
        }

    }
}