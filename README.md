# Geotag.exe – Command-Line Geotagging Tool

**Geotag.exe** is a simple C++ terminal application for geotagging images using metadata from a CSV file.
Built with Visual Studio Code and using Exiv2 as a third-party library.

This README describes how the **terminal version** of Geotag.exe works and how to use it.

---

## How It Works

The user runs the program from the terminal by calling Geotag.exe with several arguments.
- Example usage:
  - .\Geotag.exe D:\Pirovac D:\csv_folder\pirovac.csv ',' 90 1 2 3 4 5 6 7 8 9
  - .\Geotag.exe D:\Plesmo_Fotke D:\Plesmo_Fotke\CSV\Plesmo.csv ',' 45 1 2 3 4 7 6 5 -1 -1
  - .\Geotag.exe D:\Samobor_slike D:\Samobor\CSV\Samobor.csv ' ' 0 1 -1 4 2 5 3 -1 -1 -1

---

## Arguments
The program expects 14 arguments in total:
 - Geotag.exe <image_folder> <csv_file.csv> <delimiter> <image_name.jpg> <lon> <lat> <altBaro> <roll> <pitch> <yaw> <time> <altGPS>

- Explanation of arguments:
 - **image_folder**          -> Folder with the images (.jpg)
 - **csv_file.csv**          -> CSV file containing metadata (.csv)
 - **delimiter**             -> Separator between columns (',' or ' ' or '\\t')
 - **pitch_down_correction** -> Value that subtracts from pitch angle (write positive number or 0 if not used)
 - **image_name_jpg**        -> Column index of the image filename in the CSV
 - **lon**                   -> Column index of Longitude
 - **lat**                   -> Column index of Latitude
 - **altBaro**               -> Column index of Relative Altitude
 - **roll**                  -> Column index of Roll (Kappa)
 - **pitch**                 -> Column index of Pitch (Phi)
 - **yaw**                   -> Column index of Yaw (Omega)
 - **time**                  -> Column index of Timestamp (YYYY-MM-DD HH:MM:SS)
 - **altGPS**                -> Column index of GPS Altitude

---

## Exmaple of the first four arguments
- **image_folder**: Disk:\path\to\images        | D:\Pirovac\Slike
- **csv_file.csv**: Disk:\path\to\the\csv_file  | G:\Pirovac\CSV\pirovac.csv
 
- **delimiter**: delimiter can be ',' or ' ' (space) or '\t'
- **pitch_down_correction**: number that will be subtracted from the original csv pitch value 
                             (it always subtracts, if it is not used write 0)     

---

## Column Index Rules
- Column numbering starts at **1** (even though internally the program uses 0-based indexing).
- If a variable is not used, set its argument to **-1**.
  
- **Example CSV**:
  - imageName,X,Y,Z,Omega,Phi,Kappa
  - file,lon,lat,altBaro,roll,pitch,yaw,time,altGPS
  - file	 lat	 lon	 altBaro	 roll	 pitch	 yaw	 time	 altGPS
  - file lon lat altBaro roll pitch yaw time altGPS

---

## Notes
- The program currently expects exactly 14 arguments.
- Future versions may support additional CSV fields if needed.
- Geotagging starts immediately after pressing Enter.