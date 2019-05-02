#include "plugin-psu.hpp"
#include "icedb/Utils/splitSet.hpp"

namespace icedb {
	namespace plugins {
		namespace psu {
			std::string getParticleID(const std::string &filename, int32_t particle_index) {
				const size_t max_ParticleType_Size = 32;
				char particleType[max_ParticleType_Size];
				{
					if ((filename.find("aggregate")) != std::string::npos) {
						snprintf(particleType, max_ParticleType_Size, "Aggregate %05d", particle_index);
					}
					else if ((filename.find("branchedplanar")) != std::string::npos) {
						snprintf(particleType, max_ParticleType_Size, "Branched Planar %05d", particle_index);
					}
					else if ((filename.find("column")) != std::string::npos) {
						snprintf(particleType, max_ParticleType_Size, "Column %05d", particle_index);
					}
					else if ((filename.find("graupel")) != std::string::npos) {
						snprintf(particleType, max_ParticleType_Size, "Conical Graupel %05d", particle_index);
					}
					else if ((filename.find("plate")) != std::string::npos) {
						snprintf(particleType, max_ParticleType_Size, "Plate %05d", particle_index);
					}
				}
				return std::string(particleType);
			}

			particle_info getParticleInfo(const std::string &filename) {
				particle_info res;
				// Example name is psuaydinetal_aggregate_00001_HD-P1d_0.42_01_f9GHz_01_GMM.nc.
				// Explode string on '_'.
				std::vector<std::string> vSplit;
				icedb::splitSet::splitVector(filename, vSplit, '_');
				// Check that we have the correct number of split segments.
				if (vSplit.size() != 9) ICEDB_throw(icedb::error::error_types::xBadInput)
					.add<std::string>("Reason", "Cannot get the particle information from the filename. Unexpected format.");
				res.particle_type = vSplit[1];
				try { res.particle_number = boost::lexical_cast<int>(vSplit[2]); }
				catch (boost::bad_lexical_cast) {
					ICEDB_throw(icedb::error::error_types::xBadInput)
						.add<std::string>("Reason", "Cannot get the particle id number from the filename. Unexpected format.")
						.add<std::string>("Bad-id-num", vSplit[2]);
				}
				res.particle_id = getParticleID(filename, res.particle_number);
				{
					std::vector<std::string> vSplitScattMeth;
					icedb::splitSet::splitVector(vSplit[8], vSplitScattMeth, '.');
					res.scattMeth = vSplitScattMeth[0];
				}
				std::set<std::string> valid_scattMeths = { "GMM", "DDA" };
				if (!valid_scattMeths.count(res.scattMeth))
					ICEDB_throw(icedb::error::error_types::xBadInput)
					.add<std::string>("Reason", "Cannot get the scattering method from the filename. Unexpected format.")
					.add<std::string>("Bad-method", res.scattMeth);

				const auto &sFreq = vSplit[6];
				if (sFreq.size() < 5) ICEDB_throw(icedb::error::error_types::xBadInput)
					.add<std::string>("Reason", "Cannot get the frequency from the filename. Unexpected format.")
					.add<std::string>("Bad-freq", sFreq);
				if (sFreq[0] != 'f') ICEDB_throw(icedb::error::error_types::xBadInput)
					.add<std::string>("Reason", "Cannot get the frequency from the filename. Unexpected format.")
					.add<std::string>("Bad-freq", sFreq);
				size_t freqNumEnd = sFreq.find_first_not_of("0123456789.", 1);
				if (freqNumEnd == std::string::npos) ICEDB_throw(icedb::error::error_types::xBadInput)
					.add<std::string>("Reason", "Cannot get the frequency from the filename. Unexpected format.")
					.add<std::string>("Bad-freq", sFreq);
				res.freq_number_as_string = sFreq.substr(1, freqNumEnd - 1);
				if (!res.freq_number_as_string.size()) ICEDB_throw(icedb::error::error_types::xBadInput)
					.add<std::string>("Reason", "Cannot get the frequency from the filename. Unexpected format.")
					.add<std::string>("Bad-freq", sFreq);
				res.freq_units = sFreq.substr(freqNumEnd);


				return res;
			}
		}
	}
}
