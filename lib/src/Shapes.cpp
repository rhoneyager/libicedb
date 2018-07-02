#include <string>
#include <sstream>
#include <iostream>
#include "../icedb/shape.hpp"
#include "../icedb/versioning/versioning.hpp"

namespace icedb {
	namespace Shapes {
		bool NewShapeRequiredProperties::isValid(std::ostream *out) const {
			bool good = true;

			if (number_of_particle_scattering_elements == 0) {
				good = false;
				if (out) (*out) << "The number of scattering elements is not set." << std::endl;
			}
			if (number_of_particle_constituents == 0) {
				good = false;
				if (out) (*out) << "The number of particle constituents is not set." << std::endl;
			}
			if (!particle_id.size()) {
				good = false;
				if (out) (*out) << "Particle ID is not set." << std::endl;
			}
			
			if (this->particle_scattering_element_coordinates.empty()) {
				good = false;
				if (out) (*out) << "particle_scattering_element_coordinates is not set. "
					"Particles need to have scattering elements." << std::endl;
			}
			else if (particle_scattering_element_coordinates.size() != (3 * number_of_particle_scattering_elements)) {
				good = false;
				if (out) (*out) << "particle_scattering_element_coordinates has "
					"the wrong dimensions. It should have dimensions of "
					"[number_of_particle_scattering_elements][3], yielding a total of "
					<< 3 * number_of_particle_scattering_elements << " elements. "
					"Instead, it currently has " << particle_scattering_element_coordinates.size()
					<< " elements." << std::endl;
			}
			if (this->number_of_particle_constituents == 0) {
				good = false;
				if (out) (*out) << "number_of_particle_constituents is not set. "
					"Particles need to have a composition." << std::endl;
			}
			

			return good;
		}

		bool NewShapeRequiredProperties::requiresOptionalPropertiesStruct() const {
			bool req = false;
			if (this->number_of_particle_constituents > 1) req = true;
			return req;
		}

		bool NewShapeCommonOptionalProperties::isValid(
			gsl::not_null<const NewShapeRequiredProperties*> required, std::ostream *out) const
		{
			bool good = true;

			if (!this->particle_scattering_element_number.empty()) {
				if (required->number_of_particle_scattering_elements != this->particle_scattering_element_number.size()) {
					good = false;
					if (out) (*out) << "number_of_particle_scattering_elements is not equal to particle_scattering_element_number." << std::endl;
				}
			}
			if (required->number_of_particle_constituents > 1) {
				if (this->particle_constituent_number.empty() || this->particle_constituent_number.size() != required->number_of_particle_constituents) {
					good = false;
					if (out) (*out) << "particle_constituent_number is not set. "
						"This is an essential dimension scale that the rest of the particle "
						"data depends on." << std::endl;
				}

				if (this->particle_scattering_element_composition_whole.empty() 
					&& this->particle_scattering_element_composition_fractional.empty())
				{
					good = false;
					if (out) (*out) << "particle_scattering_element_composition_whole and "
						"particle_scattering_element_composition_fractional are not set, but one is required. "
						<< std::endl;
				}

				if (!this->particle_scattering_element_composition_whole.empty()
					&& !this->particle_scattering_element_composition_fractional.empty())
				{
					good = false;
					if (out) (*out) << "particle_scattering_element_composition_whole and "
						"particle_scattering_element_composition_fractional are both set, but only one is allowed. "
						<< std::endl;
				}

				if (!this->particle_scattering_element_composition_whole.empty()) {
					if (this->particle_scattering_element_composition_whole.size()
						!= required->number_of_particle_scattering_elements)
					{
						good = false;
						if (out) (*out) << "particle_scattering_element_composition_whole "
							"has the wrong size. It should have a size of number_of_particle_scattering_elements."
							<< std::endl;
					}
				}
				if (!this->particle_scattering_element_composition_fractional.empty()) {
					if (this->particle_scattering_element_composition_fractional.size() !=
						(required->number_of_particle_scattering_elements * required->number_of_particle_constituents)) {
						good = false;
						if (out) (*out) << "particle_scattering_element_composition_fractional has "
							"the wrong dimensions. It should have dimensions of "
							"[number_of_particle_scattering_elements][number_of_particle_constituents], yielding a total of "
							<< required->number_of_particle_scattering_elements * required->number_of_particle_constituents << " elements. "
							"Instead, it currently has " << this->particle_scattering_element_composition_fractional.size()
							<< " elements." << std::endl;
					}
				}
			}
			
			if (this->particle_scattering_element_radius.size()) {
				if (this->particle_scattering_element_radius.size() != required->number_of_particle_scattering_elements) {
					good = false;
					if (out) (*out) << "particle_scattering_element_radius has the wrong size. "
						"It should have dimensions of [number_of_particle_scattering_elements]. "
						"particle_scattering_element_radius has a current size of " << particle_scattering_element_radius.size()
						<< ", and this should be " << required->number_of_particle_scattering_elements
						<< "." << std::endl;
				}
			}

			if (particle_constituent_single_name.size() && required->number_of_particle_constituents != 1) {
				good = false;
				if (out) (*out) << "particle_constituent_single_name is a valid attribute only when a single non-ice constituent exists."
					<< std::endl;
			}

			if (particle_constituent_single_name.size() && this->particle_constituent_name.size()) {
				good = false;
				if (out) (*out) << "particle_constituent_single_name and particle_constituent_name are mutually exclusive."
					<< std::endl;
			}

			if (this->particle_constituent_name.size()) {
				if (this->particle_constituent_name.size() != required->number_of_particle_constituents) {
					good = false;
					if (out) (*out) << "particle_constituent_name has the wrong size. "
						"It should have dimensions of [number_of_particle_constituents]. "
						<< std::endl;
				}
			}

			if (required->number_of_particle_constituents > 1 && this->particle_constituent_name.empty()) {
				good = false;
				if (out) (*out) << "number_of_particle_constituents > 1, so particle_constituent_name "
					"is required." << std::endl;
			}

			return good;
		}

		Shape::~Shape() {}
		const std::string Shape::_icedb_obj_type_shape_identifier = "shape";
		const uint16_t Shape::_icedb_current_shape_schema_version = 0;

		bool Shape::isShape(HH::HH_hid_t group) {
			// Get link type. If this is a group, then check the attributes.
			H5O_info_t oinfo;
			herr_t err = H5Oget_info(group(), &oinfo);
			if (err < 0) return false;
			if (oinfo.type != H5O_type_t::H5O_TYPE_GROUP) return false;

			HH::Group g(group);

			if (!g.atts.exists("_icedb_obj_type")) return false;
			auto aType = g.atts["_icedb_obj_type"];
			if (H5Tget_class(aType.type()()) != H5T_STRING) return false;
			std::string sOType = aType.read<std::string>();
			if (sOType != Shape::_icedb_obj_type_shape_identifier) return false;
			return true;
		}
		bool Shape::isShape() const {
			return isShape(get());
		}

		bool Shape::isValid(std::ostream *out) const { return isValid(this->get(), out); }
		bool Shape::isValid(HH::HH_hid_t gid, std::ostream *out) {
			/// Check for the existence of the standard tables, dimensions and attributes, and that
			/// they have the appropriate sizes.

			/// \todo Finish this, and turn checks on attributes and variables into template functions!!!

			bool good = true;
			if (!isShape(gid)) {
				good = false;
				if (out) (*out) << "This is not a valid shape. Missing the appropriate icedb identifying attribute." << std::endl;
				return good;
			}
			auto group = HH::Group(gid);

			if (!group.atts.exists("particle_id")) {
				good = false;
				if (out) (*out) << "Missing the particle_id attribute." << std::endl;
			}
			else if (H5Tget_class(group.atts["particle_id"].type()()) != H5T_STRING) {
				good = false;
				if (out) (*out) << "The particle_id attribute has the wrong type." << std::endl;
			}
			if (out) (*out) << "TODO: Finish these checks!" << std::endl;
			return good;
		}

		Shape Shape::createShape(
			HH::HH_hid_t baseGrpID,
			gsl::not_null<const char*> shapeGrp,
			gsl::not_null<const NewShapeRequiredProperties*> required,
			const NewShapeCommonOptionalProperties* optional)
		{
			Group base(baseGrpID);
			if (base.exists(shapeGrp)) {
				const auto grp = base.open(shapeGrp);
				return createShape(grp.get(), required, optional);
			}
			else {
				const auto grp = base.create(shapeGrp);
				return createShape(grp.get(), required, optional);
			}
		}

		Shape Shape::createShape(
			HH::HH_hid_t newLocationAsEmptyGroup,
			gsl::not_null<const NewShapeRequiredProperties*> required,
			const NewShapeCommonOptionalProperties* optional)
		{
			Expects(required->isValid(&(std::cerr)));
			if (required->requiresOptionalPropertiesStruct()) Expects(optional);
			if (optional) Expects(optional->isValid(required));

			// newLocationAsEmptyGroup
			HH::Group res(newLocationAsEmptyGroup);

			// Write debug information
			{
				using namespace icedb::versioning;
				using std::string;
				auto libver = getLibVersionInfo();
				string sLibVer(libver->vgithash);
				res.atts.add<string>("_icedb_git_hash", sLibVer);
				res.atts.add<uint64_t>("_icedb_version",
					{ libver->vn[versionInfo::V_MAJOR], libver->vn[versionInfo::V_MINOR], libver->vn[versionInfo::V_REVISION] }, { 3 });
			}

			// Write required attributes
			res.atts.add<std::string>("_icedb_obj_type", Shape::_icedb_obj_type_shape_identifier );
			res.atts.add<uint16_t>("_icedb_shape_schema_version", Shape::_icedb_current_shape_schema_version );
			res.atts.add<std::string>("particle_id", required->particle_id );

			using namespace HH::Tags;
			using namespace HH::Tags::PropertyLists;
			constexpr size_t max_x = 40000;
			const std::vector<hsize_t> chunks2d{
				(max_x < required->number_of_particle_scattering_elements) ?
				max_x : required->number_of_particle_scattering_elements, 3 };

			auto pl2d = HH::PL::PL::createDatasetCreation().setDatasetCreationPList<uint64_t>(
				//t_CompressionType(HH::PL::CompressionType::ANY)
				t_Chunking({ chunks2d[0], chunks2d[1] })
				);
			auto pl1d = pl2d.clone().setDatasetCreationPList<uint64_t>(t_Chunking({ chunks2d[0], chunks2d[1] }));

			// Write required dimensions
			auto tblPSEN = res.dsets.create<uint64_t>(
				"particle_scattering_element_number",
				{ static_cast<size_t>(required->number_of_particle_scattering_elements) });
			{
				tblPSEN.atts.add<std::string>("description", "ID number of scattering element");
				tblPSEN.atts.add<std::string>("units", "None");

				bool added = false;
				if (optional) {
					if (!optional->particle_scattering_element_number.empty()) {
						Expects(0<= tblPSEN.write<uint64_t>(optional->particle_scattering_element_number));
						added = true;
					}
				}
				if (!added) {
					// Create "dummy" element numbers and write.
					std::vector<uint64_t> dummyPSENs(required->number_of_particle_scattering_elements);
					for (size_t i = 0; i < required->number_of_particle_scattering_elements; ++i)
						dummyPSENs[i] = i + 1;
					Expects(0 <= tblPSEN.write<uint64_t>(dummyPSENs));
				}
				// NOTE: The HDF5 dimension scale specification explicitly allows for dimensions to not have assigned values.
				// However, netCDF should have these values.
				tblPSEN.setIsDimensionScale("particle_scattering_element_number");
			}

			auto tblPCN = res.dsets.create<uint8_t>("particle_constituent_number",
				{ static_cast<size_t>(required->number_of_particle_constituents) });
			{
				tblPCN.atts.add<std::string>("description", "ID number of the constituent material");
				tblPCN.atts.add<std::string>("units", "None" );
				bool added = false;
				if (optional) {
					if (!optional->particle_constituent_number.empty()) {
						Expects(0 <= tblPCN.write<uint8_t>(optional->particle_constituent_number));
						added = true;
					}
				}
				if (!added) {
					// Create "dummy" constituent numbers and write.
					std::vector<uint8_t> dummyPCNs(required->number_of_particle_constituents);
					for (size_t i = 0; i < required->number_of_particle_constituents; ++i)
						dummyPCNs[i] = static_cast<uint8_t>(i + 1);
					Expects(0 <= tblPCN.write<uint8_t>(dummyPCNs));
				}
				tblPCN.setIsDimensionScale("particle_constituent_number");
			}

			auto tblXYZ = res.dsets.create<uint8_t>("particle_axis", { 3 });
			Expects(0 <= tblXYZ.write<uint8_t>({ 0, 1, 2 }));
			tblXYZ.setIsDimensionScale("particle_axis");

			// Determine if we can store the data as integers.
			// If non-integral coordinates, no.
			// If we can store the data as integers, then write the variable using the smallest
			// available integer storage type.
			/// \todo With HDFforHumans, be sure to add support for using minimal-sized types.
			/// \todo Auto-detect if we are using integral scattering element coordinates.
			bool useInts = (required->particle_scattering_element_coordinates_are_integral) ? true : false;
			
			HH::Dataset tblPSEC(HH::HH_hid_t::dummy()); // = res.dsets.create<uint8_t>("particle_axis", { 3 });
			if (useInts) {
				std::vector<int32_t> crds_ints(required->number_of_particle_scattering_elements*3);
				for (size_t i = 0; i < crds_ints.size(); ++i)
					crds_ints[i] = gsl::narrow_cast<int32_t>(required->particle_scattering_element_coordinates[i]);
				tblPSEC = res.dsets.create<int32_t>(
					t_name("particle_scattering_element_coordinates"),
					t_dimensions({ static_cast<size_t>(required->number_of_particle_scattering_elements), 3 }),
					t_DatasetCreationPlist(pl2d())
					);
				Expects(0 <= tblPSEC.write<int32_t>(crds_ints));
				
			} else {
				tblPSEC = res.dsets.create<float>(
					t_name("particle_scattering_element_coordinates"),
					t_dimensions({ static_cast<size_t>(required->number_of_particle_scattering_elements), 3 }),
					t_DatasetCreationPlist(pl2d())
					);
				Expects(0 <= tblPSEC.write<float>(required->particle_scattering_element_coordinates));
			}
			tblPSEC.setDims(tblPSEN, tblXYZ);
			tblPSEC.atts.add<std::string>("description", "Cartesian coordinates (x,y,z) of the center of the scattering element (dipole position, center of sphere, etc.)");
			tblPSEC.atts.add<std::string>("units", "None");
			//tblPSEC->writeAttribute<std::string>("comments", { 1 }, {
			//	"Equivalent to the coordinates given in a DDA shape-file (x,y,z-dimension) "
			//	"so scattering computations can be easily repeated with the same structure; "
			//	"for sphere methods the coordinates describe the center location of the sphere." });

			if (optional) {

				// TODO: if (optional->particle_constituent_name.size()) {}
				if (optional->particle_constituent_single_name.size()) {
					res.atts.add<std::string>("particle_single_constituent_name",
						optional->particle_constituent_single_name);
					res.atts.add<std::string>("particle_single_constituent_name__description",
						"This 3d structure is entirely composed of this material.");
				}

				if (optional->particle_scattering_element_composition_fractional.size()) {
					//const std::vector<size_t> cs{
					//	(max_x < required->number_of_particle_scattering_elements) ?
					//	max_x : required->number_of_particle_scattering_elements,
					//	static_cast<size_t>(required->number_of_particle_constituents)
					//};
					auto tblPSEC2a = res.dsets.create<float>(
						t_name("particle_scattering_element_composition_fractional"),
						t_dimensions({ static_cast<size_t>(required->number_of_particle_scattering_elements),
							static_cast<size_t>(required->number_of_particle_constituents) }),
						t_DatasetCreationPlist(pl2d())
						);
					Expects(0 <= tblPSEC2a.write<float>(optional->particle_scattering_element_composition_fractional));
					tblPSEC2a.setDims(tblPSEN, tblPCN);
					tblPSEC2a.atts.add<std::string>("description", "Mass fractions of each constituent for each scattering element.");
					tblPSEC2a.atts.add<std::string>("units", "None" );
				}

				if (optional->particle_scattering_element_composition_whole.size()) {
					auto tblPSEC2b = res.dsets.create<uint8_t>(
						t_name("particle_scattering_element_composition_whole"),
						t_dimensions({ static_cast<size_t>(required->number_of_particle_scattering_elements)}),
						t_DatasetCreationPlist(pl1d())
						);
					tblPSEC2b.setDims(tblPSEN);
					tblPSEC2b.atts.add<std::string>("description", "The constituent material ID for each scattering element.");
					tblPSEC2b.atts.add<std::string>("units", "None" );
				}

				// Write common optional attributes
				if (optional->particle_scattering_element_spacing > 0) {
					res.atts.add<float>("particle_scattering_element_spacing", optional->particle_scattering_element_spacing);
					res.atts.add<std::string>("particle_scattering_element_spacing__description", "Physical spacing between adjacent grid points");
					res.atts.add<std::string>("particle_scattering_element_spacing__units", "m");
				}

				// Write common optional variables
				if (optional->particle_scattering_element_radius.size()) {
					auto tblPSER = res.dsets.create<float>(
						t_name("particle_scattering_element_radius"),
						t_dimensions({ static_cast<size_t>(required->number_of_particle_scattering_elements) }),
						t_DatasetCreationPlist(pl1d())
						);
					Expects(0 <= tblPSER.write<float>(optional->particle_scattering_element_radius));
					tblPSER.setDims(tblPSEN);
					tblPSER.atts.add<std::string>("description", "Physical radius of the scattering sphere.");
					tblPSER.atts.add<std::string>("units", "m");
					//tblPSER.atts.add<std::string>("comments", "TODO: Units are under discussion. Either in meters or dimensionless and scaled by particle_scattering_element_spacing." });
				}

			}

			return Shape(res.get());
		}

	}
}
