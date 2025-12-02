//
// Created by sphdx on 5/17/25.
//

#ifndef ENTRYSTATUS_H
#define ENTRYSTATUS_H

namespace hash::detail {
    enum EntryStatus {
        EMPTY,
        // занято
        OCCUPIED,
        DELETED
    };
}

#endif //ENTRYSTATUS_H
