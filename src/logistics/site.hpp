#ifndef SITE_HPP_
#define SITE_HPP_

    #include <vector>
    #include "tools.hpp"
    #include "location.hpp"

    class Site : public Location {
        std::vector<SiteArea> areas;
        std::vector<Conveyor> conveyors;
        std::vector<Carrier> carrier;
    };

    class SiteArea : public Location {};

    class DeliveryPlatform : public SiteArea
    {

    };

    class StorageSpace : public SiteArea
    {

    };

#endif
