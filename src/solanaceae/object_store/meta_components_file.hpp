#pragma once

#include "./meta_components.hpp"

#include <solanaceae/util/bitset.hpp>

#include <entt/container/dense_map.hpp>

#include <vector>
#include <string>

// fwd enum (obj cant depend on Contact3
enum class Contact4 : uint32_t;

namespace ObjectStore::Components {

	// until i find a better name
	namespace File {

		// have bitsets
		// there will be hash trees in the future too
		// bitsets are not very flexible

		// TODO: use tag instead of bool?
		// the have all tag sure was useful
		struct TagLocalHaveAll {};

		struct LocalHaveBitset {
			BitSet have;
			// TODO: store actual size, since bitset is mult of 8
			// size_t bits {0};
		};

		// ephemeral?, not sure saving this to disk makes sense
		// tag remove have all?
		struct RemoteHaveBitset {
			struct Entry {
				bool have_all {false};
				BitSet have;
			};
			entt::dense_map<Contact4, Entry> others;
		};


		// for single files or member of a collection
		// split up into comps? more info?
		// other stuff like filesystems have?
		struct SingleInfo {
			std::string file_name; // full path relative to base
			uint64_t file_size {0};
		};

		// exists for stuff on disk
		// there is no garantie this exists if its, eg. encrypted
		struct SingleInfoLocal {
			std::string file_path;
		};

		struct Collection {
			std::vector<ObjectHandle> objs;
		};

		// entries in a collection can have SingleInfo,
		// but CollectionInfo is still required, so data might be duplicated
		struct CollectionInfo {
			// order is same as in `Collection`
			std::vector<SingleInfo> file_list;
			uint64_t total_size {0};
		};

		struct CollectionInfoLocal {
			// order is same as in `Collection`
			// might contain empty entries
			std::vector<SingleInfoLocal> file_list;
		};

		// img stuff
		struct FrameDims {
			uint16_t w {0u};
			uint16_t h {0u};
		};
		// struct ThumbHash {
		// struct TagNotImage {}; ???


		// receiving/sending/paused/canceled/stopped whatever
		// think about this more

#if 0
		// TODO: make an event instead/keeps on messages?
		// TODO: rename to start? or set or ...
		struct ActionAccept {
			std::string save_to_path;
			bool path_is_file = false; // if the path is not the folder to place the file into, overwrites the name
		};
#endif

	} // File

	namespace Ephemeral {

		namespace File {

			struct DownloadPriority {
				// download/retreival priority in comparison to other objects
				// not all backends implement this
				// priority can be weak, meaning low priority DLs will still get transfer activity, just less often
				enum class Priority {
					HIGHEST,
					HIGH,
					NORMAL,
					LOW,
					LOWEST,
				} p = Priority::NORMAL;
			};

			struct ReadHeadHint {
				// points to the first byte we want
				// this is just a hint, that can be set from outside
				// to guide the sequential "piece picker" strategy
				// ? the strategy *should* set this to the first byte we dont yet have
				// ???
				uint64_t offset_into_file {0u};
			};

			// this is per object/content
			// more aplicable than "separated", so should be supported by most backends
			struct TransferStats {
				// in bytes per second
				float rate_up {0.f};
				float rate_down {0.f};

				// bytes
				uint64_t total_up {0u};
				uint64_t total_down {0u};
			};

#if 0
			struct TransferStatsSeparated {
				entt::dense_map<Contact3, TransferStats> stats;
			};
#endif

			// split into up and down?
			struct TagTransferPaused {};

			// TODO: big todo, this needs to become some kind of event
			// TODO: rename to start? or set or ...
			struct ActionTransferAccept {
				std::string save_to_path;
				bool path_is_file = false; // if the path is not the folder to place the file into, overwrites the name
			};

		} // File

	} // Ephemeral

	namespace F = File;

} // ObjectStore::Components

#include "./meta_components_file_id.inl"

