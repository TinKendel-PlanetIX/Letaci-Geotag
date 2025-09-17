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

/**
 *  Splits the string to a given delimiter (',' or '\t' or ' ') and removes 
 *  " \t\r\n" characters before and after the string.
 */
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

/**
 *  Gets the value from the csv.
 */
std::string get_value(const std::vector<std::string> &row, int column_index)
{
    if (column_index == -1)
    {
        return {};
    }
    else if (column_index < row.size())
    {
        return row[column_index];   
    }

    //std::cerr << "Index is larger than the size of the CSV row.\n";
    return {};
}

int main(int argc, char* argv[])
{
    if (argc != 13)
    {
        std::cerr << "Example usage: <image_folder> <csv_file.csv> <delimiter> "
                     "<image_name.jpg> <lon> <lat> <altBaro> <roll> <pitch> <yaw> <time> <altGPS>" 
                     << '\n';

        std::cerr << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"
                     "List of Variables:\n"
                     "  <image_folder>   -> Folder with the images (.jpg)\n"
                     "  <csv_file.csv>   -> CSV file containig metadata (.csv)\n"
                     "  <delimiter>      -> Seperator between columns (',' or ' ' or '\t')\n"
                     "  <image_name_jpg> -> Column index of the image filename in the CSV\n"
                     "  <lon>            -> Column index of Longitude\n"
                     "  <lat>            -> Column index of Latitude\n"
                     "  <altBaro>        -> Column index of Relative Altitude\n"
                     "  <roll>           -> Column index of Roll (Kappa)\n"
                     "  <pitch>          -> Column index of Pitch (Phi)\n"
                     "  <yaw>            -> Column index of Yaw (Omega)\n"
                     "  <time>           -> Column index of Timestamp (YYYY-MM-DD HH:MM:SS)\n"
                     "  <altGPS>         -> Column index of GPS Altitude\n\n";


        std::cerr << "Notes:\n"
                     "  - Use number 1-9 for column positions (1 = first column).\n"
                     "  - Use -1 if a variable is not used or missing in the CSV.\n"
                     "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
    }

    std::string folder_path         = argv[1];
    std::string csv_file_path       = argv[2];
    /*
        When passing '\t' to argv it doesnt recognize it as the actual "tab" and what is worse
        it adds another \ to it for that reason we have to manualy transform the string 
        into a char that will hold '\t'
    */
    char        delimiter           {};                  
    if (argv[3][0] == '\\' && argv[3][1] == 't')
    {
        delimiter = '\t';
    }
    else
    {
        delimiter = argv[3][0];
    }
    
    std::string image_name_argv     = argv[4];
    std::string longitude_argv      = argv[5];
    std::string latitude_argv       = argv[6];
    std::string altitude_baro_argv  = argv[7];
    std::string roll_argv           = argv[8];
    std::string pitch_argv          = argv[9];
    std::string yaw_argv            = argv[10];
    std::string time_argv           = argv[11];
    std::string altitude_gps_argv   = argv[12];

    std::ifstream data_file(csv_file_path);
    if (!data_file.is_open()) 
    {
        std::cerr << "Could not open file: " << csv_file_path << std::endl;
        return 1;
    }

    std::string header_line {};
    if (!std::getline(data_file, header_line))
    {
        std::cerr << "CSV file is empty!\n";
        return 1;
    }

    // Register namespaces
    Exiv2::XmpProperties::registerNs("http://ns.drone-dji.com", "drone-dji");
    Exiv2::XmpProperties::registerNs("http://pix4d.com/camera/1.0", "Camera");

    int tagged_images_counter {0};
    int total_number_of_images {0};

    std::string line {};
    while (std::getline(data_file, line)) 
    {
        auto row = split_csv(line, delimiter);
   
        if(row.empty())
        {
            continue;
        }

        // Subtract by one becuase none devs dont know arrays or strings start indexing from 0 not from 1
        std::string image_name          = get_value(row, std::stoi(image_name_argv) - 1);
        std::string longitude           = get_value(row, std::stoi(longitude_argv) - 1);
        std::string latitude            = get_value(row, std::stoi(latitude_argv) - 1);
        std::string absolute_altitude   = get_value(row, std::stoi(altitude_gps_argv) - 1);
        std::string relative_altitude   = get_value(row, std::stoi(altitude_baro_argv) - 1);
        std::string yaw                 = get_value(row, std::stoi(yaw_argv) - 1);
        std::string pitch               = get_value(row, std::stoi(pitch_argv) - 1);
        std::string roll                = get_value(row, std::stoi(roll_argv) - 1);
        std::string time                = get_value(row, std::stoi(time_argv) - 1); //YYYY-MM-DD HH:MM:SS

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

                if (!longitude.empty())
                {
                    // Convert longitude before printing
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

                // Convert latitude before printing
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

                if (!absolute_altitude.empty())
                {
                    exifData["Exif.GPSInfo.GPSAltitude"] = absolute_altitude;
                    xmpData["Xmp.drone-dji.AbsoluteAltitude"] = absolute_altitude;
                }

                if (!relative_altitude.empty())
                {
                    exifData["Exif.GPSInfo.GPSAltitudeRef"] = "0";
                    xmpData["Xmp.drone-dji.RelativeAltitude"] = relative_altitude;
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

                total_number_of_images++;
                tagged_images_counter++;
                std::cout << "Successfully geotagged: " << image_path << std::endl;
            }
            catch (const std::exception& e) 
            {
                total_number_of_images++;
                std::cerr << "Error processing " << image_path << ": " << e.what() << std::endl;
            }
        }
    }

    std::cout << "Successfully geotagged: " << tagged_images_counter << " / " 
              << total_number_of_images << " images." << '\n';
    return 0;
}