/// \brief Provides silo file IO
#define _SCL_SECURE_NO_WARNINGS

#include <array>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <tuple>
#include <HH/Groups.hpp>
#include "plugin-silo.hpp"
#include "WritePoints.h"


namespace icedb {
	namespace plugins {
		namespace silo {
			DBdatatype convertHDF5datatypeToSiloDataType(HH::HH_hid_t ht) {
				using namespace HH;
				static const std::vector<std::pair<HH::HH_hid_t, DBdatatype > > m = {
					{HH::GetHDF5Type<int>(), DB_INT },
					{HH::GetHDF5Type<float>(), DB_FLOAT},
					{ HH::GetHDF5Type<double>(), DB_DOUBLE },
					{ HH::GetHDF5Type<char>(), DB_CHAR },
					{ HH::GetHDF5Type<long>(), DB_LONG },
					{ HH::GetHDF5Type<long long>(), DB_LONG_LONG }
				};
				for (const auto &i : m) {
					if (H5Tequal(i.first(), ht()) > 0) return i.second;
				}
				ICEDB_throw(icedb::error::error_types::xInvalidRange)
					.add("Reason", "SILO does not support this data type");
				return DB_INT;
			}
		}
	}
	namespace registry {
		using std::shared_ptr;
		using namespace icedb::plugins::silo;


		template<>
		shared_ptr<IOhandler>
			write_file_type_multi<icedb::Shapes::Shape>
			(shared_ptr<IOhandler> sh, shared_ptr<IO_options> opts,
				const icedb::Shapes::Shape * s)
				//const std::shared_ptr<const icedb::Shapes::Shape> s)
		{
			std::string filename = opts->filename();
			IOhandler::IOtype iotype = opts->iotype();

			using std::shared_ptr;
			
			// std::shared_ptr<silo_handle> h = construct_handle(sh, std::string(PLUGINID), [&]() {
			//	return std::shared_ptr<silo_handle>(new silo_handle(filename.c_str(), iotype));;
			//})
			std::shared_ptr<silo_handle> h;
			if (!sh)
				h = std::shared_ptr<silo_handle>(new silo_handle(filename.c_str(), iotype));
			else {
				if (std::string(sh->getId()) != std::string(PLUGINID)) ICEDB_throw(icedb::error::error_types::xDuplicateHook);
				h = std::dynamic_pointer_cast<silo_handle>(sh);
			}

			//bool isShape = s->isShape();
			//bool isValid = s->isValid();
			// Find the dataset that has the points.
			//auto dPSEN = s->dsets.open("particle_scattering_element_number");
			//auto dXYZ = s->dsets.open("particle_axis");
			//auto dPCN = s->dsets.open("particle_constituent_number");
			//auto tPSEC = s->dsets["particle_scattering_element_coordinates"];

			// Open every dataset. 
			auto ds = s->dsets.openAll();
			auto dPSEN = ds["particle_scattering_element_number"];
			std::map<std::string, HH::Dataset> mToSilo;
			// Collect all datasets that use particle_scattering_element_number as the first dimension.
			// Take each dataset, and, if it has the appropriate columns, then list it for silo export.
			for (const auto &d : ds) {
				if (d.second.isDimensionScale()) continue;
				if (d.second.isDimensionScaleAttached(dPSEN, 0)) {
					auto dn = d.second.getDimensions().dimensionality;
					if ((dn == 1) || (dn == 3)) mToSilo.insert(d);
				}
			}

			// The grid is based on the "particle_scattering_element_coordinates" dataset.
			// If the coordinates are integers, then we can write a rectilinear mesh as the grid.
			// If the coordinates are non-integral, then we can write a point mesh
			// TODO:	Add support for writing the mesh at a user-specified resolution.
			auto tPSEC = ds["particle_scattering_element_coordinates"];
			auto dims = tPSEC.getDimensions().dimsCur;
			Expects(dims.size() == 2);
			const char* dimLabels[] = { "X", "Y", "Z" };
			if (tPSEC.isOfType<int32_t>() > 0) {
				Eigen::Array<int32_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> crds((int)(dims[0]), (int)(dims[1]));
				Expects(0 <= tPSEC.read<int32_t>(gsl::span<int32_t>(crds.data(), crds.size())));
				auto ptmesh = h->file->createPointMesh<int32_t>("P_particle_scattering_element_coordinates", crds, dimLabels);

				for (const auto & d : mToSilo) {
					std::string units = "Unspecified"; // TODO: Query attribute and add.
					if (d.second.atts.exists("units"))
					{
						d.second.atts.open("units");
					}
					auto silo_datatype = convertHDF5datatypeToSiloDataType(d.second.type());
					ptmesh->writeData(d.first.c_str(), nullptr, 0, silo_datatype, units.c_str());

					//if (d.second.isOfType<int>() > 0) {
						//ptmesh->writeData<int>()
					//}
					//ptmesh->writeData<>(d.first.c_str(), )
				}
			}
			/*else if (tPSEC.isOfType<float>() > 0) {
				Eigen::Array<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> crds((int)(dims[0]), (int)(dims[1]));
				Expects(0 <= tPSEC.read<float>(gsl::span<float>(crds.data(), crds.size())));
				auto ptmesh = h->file->createPointMesh<float>("P_particle_scattering_element_coordinates", crds, dimLabels);
			}*/
			else ICEDB_throw(icedb::error::error_types::xBadInput)
				.add("Reason", "Wrong table type. Should never happen at this point in the code.");
			//tPSEC.read()
			
			//h->file->createPointMesh("P_Particle_Scattering_Element_Coordinates", )

			//h->file->createPointMesh
			

			// Find the others that use "particle_scattering_element_number" as a dimension.
			// These datasets can be written.

			/*
			std::string meshname("Points");
			Eigen::MatrixXf lPts(s->latticePts.rows(), s->latticePts.cols());
			lPts = s->latticePts;
			const char* axislabels[] = { "x", "y", "z" };
			const char* axisunits[] = { "dipoles", "dipoles", "dipoles" };

			std::string dielsName = meshname;
			dielsName.append("_Dielectrics");
			std::string indexName = meshname;
			indexName.append("_Point_IDs");
			//auto pm = h->file->createPointMesh<float>(meshname.c_str(), lPts, axislabels, axisunits);

			// meshPadding adds N cells before and after mins and maxs.
			// Useful when adding colocated mesh information.
			int meshPadding = opts->getVal<int>("meshPadding", 2);

			// Create a 3d mesh, also for the dielectrics
			Eigen::Array3i mins = s->mins.cast<int>(), maxs = s->maxs.cast<int>();
			// Doing this so that the ends of the shape do not get chopped off
			mins -= meshPadding * Eigen::Array3i::Ones();
			maxs += meshPadding * Eigen::Array3i::Ones();
			Eigen::Array3i span = maxs - mins + 1;
			int meshSize = span.prod();
			std::cerr << "Writing silo file with mins " << mins.transpose()
				<< " maxs " << maxs.transpose() << " span " << span.transpose() << std::endl;
			auto getCoords = [&](int i)->Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic>
			{
				Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> crd;
				crd.resize(1, 3);
				// Iterate first over z, then y, then x
				crd(0) = i % span(0);
				crd(2) = i / (span(0)*span(1));
				crd(1) = (int)(i - (crd(2)*span(0)*span(1))) / span(0); // it's not (i - i), as x involves an INTEGER division!
				crd(0) = crd(0) + mins(0);
				crd(1) = crd(1) + mins(1);
				crd(2) = crd(2) + mins(2); //+= mins;
				return crd;
			};
			auto getIndex = [&](Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> i) -> int
			{
				// Iterate first over z, then y, then x
				int res = 0;
				i(0) = i(0) - mins(0);
				i(1) = i(1) - mins(1);
				i(2) = i(2) - mins(2);
				//int ip = i(0);
				//i(0) = i(2); i(2) = ip;
				//i -= mins;
				//res = span(2) * span(1) * i(0);
				//res += span(2) * i(1);
				//res += i(2);
				res = span(0) * span(1) * i(2);
				res += span(0) * i(1);
				res += i(0);
				return res;
			};

			Eigen::VectorXf xs(span(0), 1), ys(span(1), 1), zs(span(2), 1);
			xs.setLinSpaced(mins(0), maxs(0));
			ys.setLinSpaced(mins(1), maxs(1));
			zs.setLinSpaced(mins(2), maxs(2));

			int dimsizes[] = { span(0), span(1), span(2) };
			const float *dims[] = { xs.data(), ys.data(), zs.data() };
			auto mesh = h->file->createRectilinearMesh<float>(
				"Shp_Mesh",
				3, dims, dimsizes,
				axislabels, axisunits);

			auto writeMeshData = [&](
				const char* varName,
				const Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> &coords,
				const Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> &indata,
				const char* varUnits) {
				Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> data(meshSize, indata.cols());
				data.setZero();
				int j = 0;
				std::cerr << "\tWriting mesh " << varName << std::endl;
				std::cerr << "\t\tHas " << coords.rows() << " coords, "
					"and " << indata.rows() << " points" << std::endl;
				if (coords.rows() != indata.rows()) {
					std::cerr << "\t\tMISMATCH. SKIPPING." << std::endl;
					return;
				}
				//std::cerr << "\tHas min " << mins.transpose() << " with span "
				//	<< span.transpose() << std::endl;
				for (int i = 0; i < indata.rows(); ++i)
				{
					Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> a, b;
					a.resize(1, 3); b.resize(1, 3);
					a = coords.block(i, 0, 1, 3);
					int index = getIndex(a);
					b = getCoords(index);
					//std::cerr << i << " " << a << " -> "
					//	<< index << " -> " << b << std::endl;
					if ((index < 0) || (index >= meshSize)) {
						std::cerr << "Silo mesh is too small for data size "
							<< "for variable " << varName << std::endl;
						std::cerr << "Mins " << mins.transpose()
							<< " Maxs " << maxs.transpose()
							<< " Span " << span.transpose()
							<< " prod " << meshSize << std::endl;
						std::cerr << "Failed at point i " << i << ": " << a
							<< " with index " << index << std::endl;
						return;
					}
					++j;
					//data.block(i,3,1,indata.cols()-3)
					//	= indata.block(i,3,1,indata.cols()-3);
					data.block(index, 0, 1, indata.cols())
						= indata.block(i, 0, 1, indata.cols());
				}
				if (meshSize < j) {
					std::cerr << "Silo mesh data write failed for variable "
						<< varName << std::endl;
					std::cerr << "MeshSize " << meshSize << " != j " << j << std::endl;
					return;
				}
				mesh->writeData<float>(varName, data, varUnits);
			};

			writeMeshData("Shp_Mesh_Dielectrics3",
				s->latticePts, s->latticePtsRi, "None");
			writeMeshData("Shp_Mesh_Dielectrics",
				s->latticePts,
				s->latticePtsRi.block(0, 0, s->numPoints, 1), "None");
			//double fScale = opts->getVal<double>("dielScalingFactor", 1.);
			//Eigen::ArrayXf mDielsScaled =
			//	s->latticePtsRi.block(0,0,s->numPoints,1) / fScale;
			//writeMeshData("Shp_Mesh_Scaled",
			//	s->latticePts, mDielsScaled, "None");
			writeMeshData("Shp_Mesh_Indices",
				s->latticePts, s->latticeIndex.cast<float>(), "None");

			// Write the extra matrices
			for (const auto &extras : s->latticeExtras)
			{
				std::string varname = meshname;
				varname.append("_");
				varname.append(extras.first);
				std::cerr << "Writing supplemental mesh " << varname << std::endl;
				std::cerr << "\thas " << extras.second->rows() << " rows and "
					<< extras.second->cols() << " columns." << std::endl;
				if (extras.second->cols() < 4) {
					writeMeshData(varname.c_str(),
						s->latticePts,
						*(extras.second.get()), "Unknown");
					//pm->writeData<float>(varname.c_str(), extras.second->data(), "Unknown");
				}
				else {
					Eigen::MatrixXf pts = extras.second->block(0, 0, extras.second->rows(), 3);
					// Extract the values
					size_t ndims = extras.second->cols() - 3;
					Eigen::MatrixXf vals = extras.second->block(0, 3, extras.second->rows(), ndims);
					writeMeshData(varname.c_str(),
						pts, vals, "Unknown");
					//auto npm = h->file->createPointMesh<float>(meshname2.c_str(), pts, axislabels, axisunits);
					//npm->writeData<float>(varname.c_str(), vals, "Unknown");
				}
			}
			*/
			return h; // Pass back the handle
		}


	}
}
