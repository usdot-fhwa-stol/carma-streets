#include <iostream>
#include <vector>
#include <string.h>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "bsm.h"

namespace message_services
{

    namespace models
    {

        bsm::bsm() : core_data()
        {
        }

        bsm::~bsm()
        {
        }

        void bsm::fromJsonObject(const rapidjson::Value &obj)
        {
            if (obj.IsObject())
            {
                for (rapidjson::Value::ConstMemberIterator obj_itr = obj.MemberBegin(); obj_itr != obj.MemberEnd(); obj_itr++)
                {

                    if (obj_itr->value.IsObject() && boost::iequals(std::string(obj_itr->name.GetString()), std::string("core_data")))
                    {
                        bsmCoreData_t core_data;
                        for (rapidjson::Value::ConstMemberIterator core_data_itr = obj_itr->value.MemberBegin(); core_data_itr != obj_itr->value.MemberEnd(); core_data_itr++)
                        {
                            if (std::string(core_data_itr->name.GetString()) == std::string("id"))
                            {
                                core_data.temprary_id = (core_data_itr->value.GetString());
                            }

                            if (std::string(core_data_itr->name.GetString()) == std::string("angle"))
                            {
                                core_data.angle = std::stof(core_data_itr->value.GetString());
                            }

                            if (std::string(core_data_itr->name.GetString()) == std::string("sec_mark"))
                            {
                                core_data.sec_mark = std::stol(core_data_itr->value.GetString());
                            }

                            if (std::string(core_data_itr->name.GetString()) == std::string("lat"))
                            {
                                core_data.latitude = std::stod(core_data_itr->value.GetString());
                            }

                            if (std::string(core_data_itr->name.GetString()) == std::string("msg_count"))
                            {
                                core_data.msg_count = std::stol(core_data_itr->value.GetString());
                            }

                            if (std::string(core_data_itr->name.GetString()) == std::string("long"))
                            {
                                core_data.longitude = std::stod(core_data_itr->value.GetString());
                            }

                            if (std::string(core_data_itr->name.GetString()) == std::string("elev"))
                            {
                                core_data.elev = std::stof(core_data_itr->value.GetString());
                            }

                            if (std::string(core_data_itr->name.GetString()) == std::string("heading"))
                            {
                                core_data.heading = std::stof(core_data_itr->value.GetString());
                            }

                            if (std::string(core_data_itr->name.GetString()) == std::string("transmission"))
                            {
                                core_data.transmission = (core_data_itr->value.GetString());
                            }

                            if (std::string(core_data_itr->name.GetString()) == std::string("speed"))
                            {
                                core_data.speed = std::stof(core_data_itr->value.GetString());
                            }

                            if (std::string(core_data_itr->name.GetString()) == std::string("size"))
                            {
                                for (rapidjson::Value::ConstMemberIterator size_itr = core_data_itr->value.MemberBegin(); size_itr != core_data_itr->value.MemberEnd(); size_itr++)
                                {
                                    if (boost::iequals(std::string(size_itr->name.GetString()), std::string("length")))
                                    {
                                        core_data.size.length = std::stol(size_itr->value.GetString());
                                    }
                                    if (boost::iequals(std::string(size_itr->name.GetString()), std::string("width")))
                                    {
                                        core_data.size.width = std::stol(size_itr->value.GetString());
                                    }
                                }
                            }

                            if (std::string(core_data_itr->name.GetString()) == std::string("accuracy"))
                            {
                                for (rapidjson::Value::ConstMemberIterator size_itr = core_data_itr->value.MemberBegin(); size_itr != core_data_itr->value.MemberEnd(); size_itr++)
                                {
                                    if (boost::iequals(std::string(size_itr->name.GetString()), std::string("orientation")))
                                    {
                                        core_data.accuracy.orientation = std::stof(size_itr->value.GetString());
                                    }
                                    if (boost::iequals(std::string(size_itr->name.GetString()), std::string("semi_major")))
                                    {
                                        core_data.accuracy.semiMajor = std::stof(size_itr->value.GetString());
                                    }
                                    if (boost::iequals(std::string(size_itr->name.GetString()), std::string("semi_minor")))
                                    {
                                        core_data.accuracy.semiMinor = std::stof(size_itr->value.GetString());
                                    }
                                }
                            }

                            if (std::string(core_data_itr->name.GetString()) == std::string("accel_set"))
                            {
                                for (rapidjson::Value::ConstMemberIterator size_itr = core_data_itr->value.MemberBegin(); size_itr != core_data_itr->value.MemberEnd(); size_itr++)
                                {
                                    if (boost::iequals(std::string(size_itr->name.GetString()), std::string("lat")))
                                    {
                                        core_data.accelSet.lat = std::stof(size_itr->value.GetString());
                                    }
                                    if (boost::iequals(std::string(size_itr->name.GetString()), std::string("long")))
                                    {
                                        core_data.accelSet.Long = std::stof(size_itr->value.GetString());
                                    }
                                    if (boost::iequals(std::string(size_itr->name.GetString()), std::string("vert")))
                                    {
                                        core_data.accelSet.vert = std::stof(size_itr->value.GetString());
                                    }
                                    if (boost::iequals(std::string(size_itr->name.GetString()), std::string("yaw")))
                                    {
                                        core_data.accelSet.yaw = std::stof(size_itr->value.GetString());
                                    }
                                }
                            }

                            if (std::string(core_data_itr->name.GetString()) == std::string("brakes"))
                            {
                                for (rapidjson::Value::ConstMemberIterator size_itr = core_data_itr->value.MemberBegin(); size_itr != core_data_itr->value.MemberEnd(); size_itr++)
                                {
                                    if (boost::iequals(std::string(size_itr->name.GetString()), std::string("abs")))
                                    {
                                        core_data.brakes.abs = std::stol(size_itr->value.GetString());
                                    }
                                    if (boost::iequals(std::string(size_itr->name.GetString()), std::string("aux_brakes")))
                                    {
                                        core_data.brakes.auxBrakes = std::stol(size_itr->value.GetString());
                                    }
                                    if (boost::iequals(std::string(size_itr->name.GetString()), std::string("brake_boost")))
                                    {
                                        core_data.brakes.brakeBoost = std::stol(size_itr->value.GetString());
                                    }

                                    if (boost::iequals(std::string(size_itr->name.GetString()), std::string("scs")))
                                    {
                                        core_data.brakes.scs = std::stol(size_itr->value.GetString());
                                    }

                                    if (boost::iequals(std::string(size_itr->name.GetString()), std::string("traction")))
                                    {
                                        core_data.brakes.traction = std::stol(size_itr->value.GetString());
                                    }
                                }
                            }
                        }
                        setCore_data(core_data);
                    }
                }
            }
        }

        bool bsm::asJsonObject(rapidjson::Writer<rapidjson::StringBuffer> *writer) const
        {
            try
            {
                rapidjson::Value val_core_data;
                writer->Key("bsmCoreData");
                writer->StartObject();
                writer->Key("temprary_id");
                writer->String(this->getCore_data().temprary_id.c_str());
                writer->String("secMark");
                writer->String(std::to_string(this->getCore_data().sec_mark).c_str());
                writer->String("latitude");
                writer->String(std::to_string(this->getCore_data().latitude).c_str());
                writer->String("longitude");
                writer->String(std::to_string(this->getCore_data().longitude).c_str());
                writer->String("elev");
                writer->String(std::to_string(this->getCore_data().elev).c_str());
                writer->String("heading");
                writer->String(std::to_string(this->getCore_data().heading).c_str());
                writer->String("transmission");
                writer->String(this->getCore_data().transmission.c_str());
                writer->String("speed");
                writer->String(std::to_string(this->getCore_data().speed).c_str());
                writer->EndObject();
                writer->EndObject();
                return true;
            }
            catch (std::exception &ex)
            {
                spdlog::critical("BSM model: asJsonObject error.: ", ex.what());
                return false;
            }
        }

        std::ostream &operator<<(std::ostream &out, bsm &bsm_obj)
        {
            out << std::fixed;
            out << std::setprecision(6);
            out << "bsm model: core_data: { temprary_id = " << bsm_obj.core_data.temprary_id << ", sec_mark = " << bsm_obj.core_data.sec_mark
                      << ", latitude= " << bsm_obj.core_data.latitude << ", longitude = " << bsm_obj.core_data.longitude
                      << ", size: {length = " << bsm_obj.core_data.size.length << "}, speed = " << bsm_obj.core_data.speed << "}" << std::endl;
            return out;
        }

        std::string bsm::generate_hash_bsm_msg_id(std::string temprary_id, long msg_count, long sec_mark)
        {
            std::stringstream hash_ss;
            hash_ss << temprary_id << msg_count << sec_mark;
            return hash_ss.str();
        }      

        bsmCoreData_t bsm::getCore_data() const
        {
            return this->core_data;
        }

        void bsm::setCore_data(bsmCoreData_t core_data)
        {
            this->core_data = core_data;
        }

    }
}