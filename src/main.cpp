#include <exiv2.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>  // For std::fabs
#include <filesystem>

#include <unordered_map>
#include <vector>
#include <algorithm>

std::vector<std::string> split_csv(const std::string& line, char delimiter = ',')
{
    std::vector<std::string> result;
    std::stringstream ss(line);
    std::string token;

    while (std::getline(ss, token, delimiter))
    {
        // This is done so that there is no whitespace or tabs in the token
        token.erase(0, token.find_first_not_of(" \t\r\n"));
        token.erase(token.find_last_not_of(" \t\r\n") + 1);
        result.push_back(token);
    }

    return result;
}

std::string get_value(const std::vector<std::string> &row,
                      const std::unordered_map<std::string, size_t> &column_index,
                      const std::vector<std::string> &keys)
{
    for (auto& k : keys)
    {
        auto it = column_index.find(k);
        if (it != column_index.end() && it->second < row.size())
        {
            return row[it->second];
        }
    }
    return {};
}


int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Example usage: <image_folder> <csv_file.csv>" << '\n';
    }


    std::string folder_path = argv[1];       //"D:/Plesmo_Fotke";
    std::string csv_file_path = argv[2];      //"D:/Plesmo_Fotke/CSV/KO_Plesmo_EVOSOPK.csv";

    std::ifstream dataFile(csv_file_path);
    if (!dataFile.is_open()) 
    {
        std::cerr << "Could not open file: " << csv_file_path << std::endl;
        return 1;
    }

    std::string header_line {};
    if (!std::getline(dataFile, header_line))
    {
        std::cerr << "CSV file is empty!\n";
        return 1;
    }

    // Register namespaces
    Exiv2::XmpProperties::registerNs("http://ns.drone-dji.com", "drone-dji");
    Exiv2::XmpProperties::registerNs("http://pix4d.com/camera/1.0", "Camera");

    auto header_variables = split_csv(header_line, ',');
    // This holds the names of variables that are in the header of the csv
    // Example: file,	 lat,	 lon,	 altBaro,	 roll,	 pitch,	 yaw,	 time
    std::unordered_map<std::string, size_t> column_index; 
    for (size_t i = 0; i < header_variables.size(); i++)
    {
        std::string h = header_variables[i];
        std::transform(h.begin(), h.end(), h.begin(), ::tolower);
        column_index[h] = i;
    }

    std::string line;
    while (std::getline(dataFile, line)) 
    {
        auto row = split_csv(line, ',');
        if(row.empty())
        {
            continue;
        }
        
        std::string image_name          = get_value(row, column_index, {"imagename", "file"});
        std::string longitude           = get_value(row, column_index, {"x", "lon"});
        std::string latitude            = get_value(row, column_index, {"y", "lat"});
        std::string absolute_altitude   = get_value(row, column_index, {"altbaro"});
        std::string relative_altitude   = get_value(row, column_index, {"z", "altgps"});
        std::string yaw                 = get_value(row, column_index, {"omega", "yaw"});
        std::string pitch               = get_value(row, column_index, {"phi", "pitch"});
        std::string roll                = get_value(row, column_index, {"kappa", "roll"});
        std::string time                = get_value(row, column_index, {"time"}); //YYYY-MM-DD HH:MM:SS
    
        if(image_name.empty())
        {
            continue;
        }

        std::string image_path = folder_path + "/" + image_name;
        if (std::filesystem::exists(image_path)) 
        {
            try
            {
                auto image = Exiv2::ImageFactory::open(image_path);
                image->readMetadata();

                auto &exifData = image->exifData();
                auto &xmpData  = image->xmpData();

                // covert latitude before printing
                if (!latitude.empty())
                {
                    double latitude_float = std::stod(latitude);
                    uint32_t lat_deg = latitude_float;   // to int -> no decimal part
                    double d_lat_min = 60.0 * (latitude_float - lat_deg);  // * decimal part
                    uint32_t lat_min = d_lat_min;
                    double d_lat_sec = 10000.0 * 60 * (d_lat_min - lat_min);
                    uint32_t lat_sec = d_lat_sec;
                    std::ostringstream lat;
                    lat << lat_deg << "/1 " << lat_min << "/1 " << lat_sec << "/10000";

                    exifData["Exif.GPSInfo.GPSVersionID"] = "2 3 0 0";
                    exifData["Exif.GPSInfo.GPSLatitudeRef"] = "N";
                    exifData["Exif.GPSInfo.GPSLatitude"] = lat.str();
                }

                if (!longitude.empty())
                {
                    // convert longitude before printing
                    double longitude_float = std::stod(longitude);
                    uint32_t lon_deg = longitude_float;   // to int -> no decimal part
                    double d_lon_min = 60.0 * (longitude_float - lon_deg);  // * decimal part
                    uint32_t lon_min = d_lon_min;
                    double d_lon_sec = 10000.0 * 60 * (d_lon_min - lon_min);
                    uint32_t lon_sec = d_lon_sec;
                    std::ostringstream lon;
                    lon << lon_deg << "/1 " << lon_min << "/1 " << lon_sec << "/10000";

                    exifData["Exif.GPSInfo.GPSLongitudeRef"] = "E";
                    exifData["Exif.GPSInfo.GPSLongitude"] = lon.str();
                }


                if (!relative_altitude.empty())
                {
                    // convert altitude before printing
                    // double altitude_float = std::stod(relative_altitude);
                    // uint32_t i_alt = 1000.0 * altitude_float;
                    // std::ostringstream alt;
                    // alt << i_alt << "/1000";

                    exifData["Exif.GPSInfo.GPSAltitudeRef"] = "0";
                    exifData["Exif.GPSInfo.GPSAltitude"] = relative_altitude;//alt.str();

                    xmpData["Xmp.drone-dji.RelativeAltitude"] = relative_altitude;//alt.str();
                }

                
                if (!absolute_altitude.empty())
                {
                    // double altitude_float = std::stod(absolute_altitude);
                    // uint32_t i_alt = 1000.0 * altitude_float;
                    // std::ostringstream alt;
                    // alt << i_alt << "/1000";

                    xmpData["Xmp.drone-dji.AbsoluteAltitude"] = absolute_altitude; //alt.str();
                }
            

                if (!yaw.empty())
                {
                    xmpData["Xmp.drone-dji.GimbalYawDegree"] = yaw;
                }


                if (!pitch.empty())
                {
                    xmpData["Xmp.drone-dji.GimbalPitchDegree"] = pitch; 
                }


                if(!roll.empty())
                {
                    xmpData["Xmp.drone-dji.GimbalRollDegree"] = roll;
                }


                if(!time.empty())
                {
                    xmpData["Xmp.drone-dji.ModifyDate"] = time;
                }

                exifData["Exif.GPSInfo.GPSStatus"] = "A";
                exifData["Exif.GPSInfo.GPSMapDatum"] = "WGS-84";

                // Saving exif and xmp data to the image
                image->setExifData(exifData);
                image->setXmpData(xmpData);
                image->writeMetadata();

                std::cout << "Successfully geotagged: " << image_path << std::endl;
            }
            catch (const std::exception& e) 
            {
                std::cerr << "Error processing " << image_path << ": " << e.what() << std::endl;
            }
        }
    }

    return 0;
}