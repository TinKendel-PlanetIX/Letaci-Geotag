#Geotag.exe – Command-Line Geotagging Tool

**Geotag.exe** is a simple C++ terminal application for geotagging images using metadata from a CSV file.
Built with Visual Studio Code and using Exiv2 as a third-party library.

This README describes how the **terminal version** of Geotag.exe works and how to use it.

---

## How It Works

The user runs the program from the terminal by calling Geotag.exe with several arguments.
- Example usage:
  - .\Geotag.exe D:\slike_s_misije D:\csv_folder\gradina.csv ',' 1 2 3 4 5 6 7 8 9
  - .\Geotag.exe D:\Plesmo_Fotke D:\Plesmo_Fotke\CSV\Plesmo.csv ',' 1 2 3 4 7 6 5 -1 -1
  - .\Geotag.exe D:\Samobor_slike D:\Samobor\CSV\Samobor.csv ' ' 1 -1 4 2 5 3 -1 -1 -1

---

## Arguments
The program expects 13 arguments in total:
 - Geotag.exe <image_folder> <csv_file.csv> <delimiter> <image_name.jpg> <lon> <lat> <altBaro> <roll> <pitch> <yaw> <time> <altGPS>

 Explanation of arguments:

 - <image_folder> → Path to folder containing the images (.jpg).
 - <csv_file.csv> → Path to the CSV file with metadata.
 - <delimiter> → Column separator in CSV. Options: ',', ' ' (space), or '\t' (tab).
 - <image_name.jpg> → Column index of the image filename in CSV.
 - <lon> → Column index of Longitude.
 - <lat> → Column index of Latitude.
 - <altBaro> → Column index of Relative Altitude.
 - <roll> → Column index of Roll (Kappa).
 - <pitch> → Column index of Pitch (Phi).
 - <yaw> → Column index of Yaw (Omega).
 - <time> → Column index of Timestamp (format: YYYY-MM-DD HH:MM:SS).
 - <altGPS> → Column index of GPS Altitude.

---

## Column Index Rules
- Column numbering starts at **1** (even though internally the program uses 0-based indexing).
- If a variable is not used, set its argument to **-1**.
- **Example CSV**:
- imageName,X,Y,Z,Omega,Phi,Kappa

---

## Example mapping:
- Geotag.exe images/ metadata.csv ',' 1 2 3 4 5 6 7 -1 -1

---

## Notes
- The program currently expects exactly 13 arguments.
- Future versions may support additional CSV fields if needed.
- Geotagging starts immediately after pressing Enter.
