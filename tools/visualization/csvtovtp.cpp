#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <vtkVersion.h>
#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkPointData.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkDoubleArray.h>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>


int main(int ac, char* av[])
{
  //Declare all variables
  int numPoints,counter;
  char c;
  double x,y,z;
  numPoints = counter = 0;
  x = y = z = 0;
  std::string sCell;
  std::string filename;

  //Add Program options (help,filename)
  namespace po = boost::program_options;
  po::options_description config("Allowed Options");
  config.add_options()
    ("help", "Help Message")
    ("filename,f", po::value<boost::filesystem::path>()->default_value("movment.csv"));
     
  po::variables_map vm;
  po::store(po::parse_command_line(ac, av, config), vm);
  po::notify(vm);
    
  if (vm.count("help")) {
      std::cout << config << "\n";
  }

  filename = vm["filename"].as<boost::filesystem::path>().c_str();
    
  //open input
  ifstream csvFile(filename);
  cout << "Read in " << filename << " as input file \n";
  
  if(!csvFile.good()){
    cout << "Empty File, ending operation, use -f to set the filepath \n";
    return 0;
  }
    
  //Get the number of points in the file ( the number of data entries)
  //replaces all the new line charcters with \n so it /shouldn't/ mess up with different file systems
  while (csvFile.get(c)){
    if ((c == '\r') || (c == '\n') || (c == '\t')){
      numPoints++;
      c = '\n';
    }
  }
    
  //"Reverses" the file
  csvFile.clear();
  csvFile.seekg(0);
    
  //Create point,id,rotation containers
  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    
  vtkSmartPointer<vtkIntArray> ids = vtkSmartPointer<vtkIntArray>::New();
    ids->SetNumberOfComponents(1);
    getline(csvFile, sCell, ',');
    ids->SetName(sCell.c_str());
    
  //Skips the xyz as that is placed into the points
  getline(csvFile, sCell, ',');
  getline(csvFile, sCell, ',');
  getline(csvFile, sCell, ',');
    
  vtkSmartPointer<vtkDoubleArray> rxs = vtkSmartPointer<vtkDoubleArray>::New();
    rxs->SetNumberOfComponents(1);
    getline(csvFile, sCell, ',');
    rxs->SetName(sCell.c_str());
  
  vtkSmartPointer<vtkDoubleArray> rys = vtkSmartPointer<vtkDoubleArray>::New();
    rys->SetNumberOfComponents(1);
    getline(csvFile, sCell, ',');
    rys->SetName(sCell.c_str());
    
  vtkSmartPointer<vtkDoubleArray> rzs = vtkSmartPointer<vtkDoubleArray>::New();
    rzs->SetNumberOfComponents(1);
    getline(csvFile, sCell, ',');
    rzs->SetName(sCell.c_str());

  vtkSmartPointer<vtkDoubleArray> rws = vtkSmartPointer<vtkDoubleArray>::New();
    rws->SetNumberOfComponents(1);
    getline(csvFile, sCell, '\n');
    rws->SetName(sCell.c_str());
    
  cout << "Read in : "<< numPoints - 1 << " points \n";
    
  while ( csvFile.good() && (counter++ < (numPoints - 1))) {
    //Get id
    getline(csvFile,sCell, ',');
    ids->InsertNextValue(stoi(sCell));
      
    //Get X
    getline(csvFile, sCell, ',');
    x = stod(sCell,NULL);
        
    //Get Y
    getline(csvFile, sCell, ',');
    y = stod(sCell,NULL);
        
    //Get Z
    getline(csvFile, sCell, ',');
    z = stod(sCell,NULL);

    //Get rx
    getline(csvFile,sCell, ',');
    rxs->InsertNextValue(stod(sCell,NULL));
      
    //Get ry
    getline(csvFile,sCell, ',');
    rys->InsertNextValue(stod(sCell,NULL));
      
    //Get rz
    getline(csvFile,sCell, ',');
    rzs->InsertNextValue(stod(sCell,NULL));
      
    //Get rw
    getline(csvFile,sCell, '\n');
    rws->InsertNextValue(stod(sCell,NULL));
    
    //cout << counter << ". ("<< x << "," << y << "," << z << ")\n";
    points->InsertNextPoint ( x, y, z );
  }
    
  //Create a polydata object that we will later add our data to
  vtkSmartPointer<vtkPolyData> polydata =
  vtkSmartPointer<vtkPolyData>::New();
    
  polydata->SetPoints(points);
  polydata->GetPointData()->AddArray(ids);
  polydata->GetPointData()->AddArray(rxs);
  polydata->GetPointData()->AddArray(rys);
  polydata->GetPointData()->AddArray(rzs);
  polydata->GetPointData()->AddArray(rws);
    
  //Write the file
  cout << "Writing file to " << vm["filename"].as<boost::filesystem::path>().stem().c_str() <<  ".vtp\n";
    
  vtkSmartPointer<vtkXMLPolyDataWriter> writer =
    vtkSmartPointer<vtkXMLPolyDataWriter>::New();
  writer->SetFileName((vm["filename"].as<boost::filesystem::path>().stem().string() + ".vtp").c_str());
  writer->SetInputData(polydata);
  writer->Write();
    
  return 0;
}
    
