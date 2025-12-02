//
// Created by sphdx on 6/23/25.
//

#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <string>
#include "../model/PersonName.h"

namespace repo {
    using ToKey = std::string(*)(const model::PersonName &, const std::string &);
}

#endif //REPOSITORY_H
