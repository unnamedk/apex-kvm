#include "options.hpp"
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include <cereal/archives/json.hpp>

using apex::config::options_t;
namespace fs = std::filesystem;

std::optional<apex::config::options_t> apex::options = std::nullopt;

options_t::options_t()
{
    auto pw = getenv( "SUDO_USER" );
    if ( !pw ) {
        pw = "idkaname"; //getenv( "USER" );
    }

    current_config = "/home" / fs::path { pw } / "Documents" / "Cheat Settings";

    if ( !fs::exists( current_config ) ) {
        fs::create_directory( current_config );
    }

    current_config /= "apex_settings.json";
    this->load();
}

void options_t::load()
{
    if ( current_config.has_filename() && !fs::exists( current_config ) ) {
        return; // no config exists, shouldn't happen
    }

    std::ifstream ifs( current_config );
    cereal::JSONInputArchive in( ifs );

    in( cereal::make_nvp( "config", *this ) );
}

void options_t::save()
{
    std::ofstream ofs( current_config, std::ios::out | std::ios::trunc );
    cereal::JSONOutputArchive out( ofs );

    out( cereal::make_nvp( "config", *this ) );
}