﻿/*
	SPDX-License-Identifier: MIT
	Copyright © 2016-2023 Amebis
*/

#pragma once

#include "compat.hpp"
#include "stream.hpp"
#include <ios>
#include <istream>
#include <ostream>

namespace stdex {
	namespace idrec {
		///
		/// Reads record ID
		///
		/// \param[in]  stream  Input stream
		/// \param[out] id      Record ID
		/// \param[in]  end     Position limit. Default is -1 (no limit).
		///
		/// \returns
		/// - \c true when succeeded
		/// - \c false otherwise
		///
		template <class T_ID>
		inline _Success_(return) bool read_id(_In_ std::istream& stream, _Out_ T_ID &id, _In_opt_ std::streamoff end = (std::streamoff)-1)
		{
			if (end == (std::streamoff)-1 || stream.tellg() < end) {
				stream.read((char*)&id, sizeof(id));
				return stream.good();
			} else
				return false;
		}

		///
		/// Reads record ID
		///
		/// \param[in]  stream  Input stream
		/// \param[out] id      Record ID
		/// \param[in]  end     Position limit. Default is -1 (no limit).
		///
		/// \returns
		/// - \c true when succeeded
		/// - \c false otherwise
		///
		template <class T_ID>
		inline _Success_(return) bool read_id(_In_ stdex::stream::basic_file& stream, _Out_ T_ID &id, _In_opt_ stdex::stream::fpos_t end = stdex::stream::fpos_max)
		{
			if (end == stdex::stream::fpos_max || stream.tell() < end) {
				stream >> id;
				return stream.ok();
			} else
				return false;
		}

		///
		/// Calculates required padding
		///
		/// \param[in] size  Actual data size
		///
		/// \return Number of bytes needed to add to the data to align it on `ALIGN` boundary
		///
		template <class T_SIZE, T_SIZE ALIGN>
		inline T_SIZE padding(_In_ T_SIZE size)
		{
			return (ALIGN - (size % ALIGN)) % ALIGN;
		}

		///
		/// Skips current record data
		///
		/// \param[in] stream  Input stream
		///
		/// \returns
		/// - \c true when successful
		/// - \c false otherwise
		///
		template <class T_SIZE, T_SIZE ALIGN>
		inline bool ignore(_In_ std::istream& stream)
		{
			// Read record size.
			T_SIZE size;
			stream.read((char*)&size, sizeof(size));
			if (!stream.good()) _Unlikely_ return false;

			// Skip the record data.
			size += padding<T_SIZE, ALIGN>(size);
			stream.ignore(size);
			if (!stream.good()) _Unlikely_ return false;

			return true;
		}

		///
		/// Skips current record data
		///
		/// \param[in] stream  Input stream
		///
		/// \returns
		/// - \c true when successful
		/// - \c false otherwise
		///
		template <class T_SIZE, T_SIZE ALIGN>
		inline bool ignore(_In_ stdex::stream::basic& stream)
		{
			// Read record size.
			T_SIZE size;
			stream >> size;
			if (!stream.ok()) _Unlikely_ return false;

			// Skip the record data.
			size += padding<T_SIZE, ALIGN>(size);
			stream.skip(size);
			if (!stream.ok()) _Unlikely_ return false;

			return true;
		}

		///
		/// Finds record data
		///
		/// \param[in] stream  Input stream
		/// \param[in] id      Record ID
		/// \param[in] end     Position limit. Default is -1 (no limit).
		///
		/// \returns
		/// - \c true when found
		/// - \c false otherwise
		///
		template <class T_ID, class T_SIZE, T_SIZE ALIGN>
		inline bool find(_In_ std::istream& stream, _In_ T_ID id, _In_opt_ std::streamoff end = (std::streamoff)-1)
		{
			T_ID _id;
			while (end == (std::streamoff)-1 || stream.tellg() < end) {
				stream.read((char*)&_id, sizeof(_id));
				if (!stream.good()) _Unlikely_ return false;
				if (_id == id) {
					// The record was found.
					return true;
				} else
					ignore<T_SIZE, ALIGN>(stream);
			}
			return false;
		}

		///
		/// Finds record data
		///
		/// \param[in] stream  Input stream
		/// \param[in] id      Record ID
		/// \param[in] end     Position limit. Default is -1 (no limit).
		///
		/// \returns
		/// - \c true when found
		/// - \c false otherwise
		///
		template <class T_ID, class T_SIZE, T_SIZE ALIGN>
		inline bool find(_In_ stdex::stream::basic_file& stream, _In_ T_ID id, _In_opt_ stdex::stream::fpos_t end = stdex::stream::fpos_max)
		{
			T_ID _id;
			while (end == stdex::stream::fpos_max || stream.tell() < end) {
				stream >> _id;
				if (!stream.ok()) _Unlikely_ return false;
				if (_id == id) {
					// The record was found.
					return true;
				} else
					ignore<T_SIZE, ALIGN>(stream);
			}
			return false;
		}

		///
		/// Writes record header
		///
		/// \param[in] stream  Output stream
		/// \param[in] id      Record ID
		///
		/// \returns  Position of the record header start in \p stream. Save for later \c close call.
		///
		template <class T_ID, class T_SIZE>
		inline std::streamoff open(_In_ std::ostream& stream, _In_ T_ID id)
		{
			std::streamoff start = stream.tellp();

			// Write ID.
			if (stream.fail()) _Unlikely_ return (std::streamoff)-1;
			stream.write((const char*)&id, sizeof(id));

			// Write 0 as a placeholder for data size.
			if (stream.fail()) _Unlikely_ return (std::streamoff)-1;
			T_SIZE size = 0;
			stream.write((const char*)&size, sizeof(size));

			return start;
		}

		///
		/// Writes record header
		///
		/// \param[in] stream  Output stream
		/// \param[in] id      Record ID
		///
		/// \returns  Position of the record header start in \p stream. Save for later \c close call.
		///
		template <class T_ID, class T_SIZE>
		inline stdex::stream::fpos_t open(_In_ stdex::stream::basic_file& stream, _In_ T_ID id)
		{
			auto start = stream.tell();

			// Write ID.
			stream << id;

			// Write 0 as a placeholder for data size.
			stream << static_cast<T_SIZE>(0);

			return start;
		}

		///
		/// Updates record header
		///
		/// \param[in] stream  Output stream
		/// \param[in] start   Start position of the record in \p stream
		///
		/// \returns  Position of the record end in \p stream
		///
		template <class T_ID, class T_SIZE, T_SIZE ALIGN>
		inline std::streamoff close(_In_ std::ostream& stream, _In_ std::streamoff start)
		{
			std::streamoff end = stream.tellp();
			T_SIZE
				size      = static_cast<T_SIZE>(end - start - sizeof(T_ID) - sizeof(T_SIZE)),
				remainder = padding<T_SIZE, ALIGN>(size);

			if (remainder) {
				// Append padding.
				static const char padding[ALIGN] = {};
				stream.write(padding, remainder);
				end += remainder;
			}

			// Update the data size.
			if (stream.fail()) _Unlikely_ return (std::streamoff)-1;
			stream.seekp(start + sizeof(T_ID));
			stream.write(reinterpret_cast<const char*>(&size), sizeof(size));
			stream.seekp(end);

			return end;
		}

		///
		/// Updates record header
		///
		/// \param[in] stream  Output stream
		/// \param[in] start   Start position of the record in \p stream
		///
		/// \returns  Position of the record end in \p stream
		///
		template <class T_ID, class T_SIZE, T_SIZE ALIGN>
		inline stdex::stream::fpos_t close(_In_ stdex::stream::basic_file& stream, _In_ stdex::stream::fpos_t start)
		{
			auto end = stream.tell();
			T_SIZE
				size      = static_cast<T_SIZE>(end - start - sizeof(T_ID) - sizeof(T_SIZE)),
				remainder = padding<T_SIZE, ALIGN>(size);

			if (remainder) {
				// Append padding.
				static const char padding[ALIGN] = {};
				stream.write_array(padding, sizeof(char), remainder);
				end += remainder;
			}

			// Update the data size.
			if (!stream.ok()) _Unlikely_ return stdex::stream::fpos_max;
			stream.seek(start + sizeof(T_ID));
			stream << size;
			stream.seek(end);

			return end;
		}

		///
		/// Helper class for read/write of records to/from memory
		///
		template <class T, class T_ID, const T_ID ID, class T_SIZE, T_SIZE ALIGN>
		class record
		{
		public:
			///
			/// Constructs the class
			///
			/// \param[in] d  Reference to record data
			///
			record(_In_ T &d) : data(d) {}

			///
			/// Constructs the class
			///
			/// \param[in] d  Reference to record data
			///
			record(_In_ const T &d) : data((T&)d) {}

			///
			/// Returns record id
			///
			static constexpr T_ID id()
			{
				return ID;
			}

			///
			/// Assignment operator
			///
			/// \param[in] r  Source record
			///
			/// \returns A const reference to this struct
			///
			const record<T, T_ID, ID, T_SIZE, ALIGN>& operator =(_In_ const record<T, T_ID, ID, T_SIZE, ALIGN> &r)
			{
				data = r.data;
				return *this;
			}

			///
			/// Writes record header
			///
			/// \param[in] stream  Output stream
			///
			/// \returns  Position of the record header start in \p stream. Save for later \c close call.
			///
			static std::streamoff open(_In_ std::ostream& stream)
			{
				return stdex::idrec::open<T_ID, T_SIZE>(stream, ID);
			}

			///
			/// Writes record header
			///
			/// \param[in] stream  Output stream
			///
			/// \returns  Position of the record header start in \p stream. Save for later \c close call.
			///
			static stdex::stream::foff_t open(_In_ stdex::stream::basic_file& stream)
			{
				return stdex::idrec::open<T_ID, T_SIZE>(stream, ID);
			}

			///
			/// Updates record header
			///
			/// \param[in] stream  Output stream
			/// \param[in] start   Start position of the record in \p stream
			///
			/// \returns  Position of the record end in \p stream
			///
			static std::streamoff close(_In_ std::ostream& stream, _In_ std::streamoff start)
			{
				return stdex::idrec::close<T_ID, T_SIZE, ALIGN>(stream, start);
			}

			///
			/// Updates record header
			///
			/// \param[in] stream  Output stream
			/// \param[in] start   Start position of the record in \p stream
			///
			/// \returns  Position of the record end in \p stream
			///
			static stdex::stream::foff_t close(_In_ stdex::stream::basic_file& stream, _In_ stdex::stream::foff_t start)
			{
				return stdex::idrec::close<T_ID, T_SIZE, ALIGN>(stream, start);
			}

			///
			/// Finds record data
			///
			/// \param[in] stream  Input stream
			/// \param[in] end     Position limit. Default is -1 (no limit).
			///
			/// \returns
			/// - \c true when found
			/// - \c false otherwise
			///
			static bool find(_In_ std::istream& stream, _In_opt_ std::streamoff end = (std::streamoff)-1)
			{
				return stdex::idrec::find<T_ID, T_SIZE, ALIGN>(stream, ID, end);
			}

			///
			/// Finds record data
			///
			/// \param[in] stream  Input stream
			/// \param[in] end     Position limit. Default is -1 (no limit).
			///
			/// \returns
			/// - \c true when found
			/// - \c false otherwise
			///
			static bool find(_In_ stdex::stream::basic_file& stream, _In_opt_ stdex::stream::foff_t end = stdex::stream::foff_max)
			{
				return stdex::idrec::find<T_ID, T_SIZE, ALIGN>(stream, ID, end);
			}

			T &data; ///< Record data reference

			///
			/// Writes record to a stream
			///
			/// \param[in] stream  Output stream
			/// \param[in] r       Record
			///
			/// \returns The stream \p stream
			///
			friend std::ostream& operator <<(_In_ std::ostream& stream, _In_ const record<T, T_ID, ID, T_SIZE, ALIGN> r)
			{
				// Parameter r does not need to be passed by reference. It has only one field (data), which is a reference itself already.

				auto start = r.open(stream);
				if (stream.fail()) _Unlikely_ return stream;
				stream << r.data;
				r.close(stream, start);

				return stream;
			}

			///
			/// Writes record to a file
			///
			/// \param[in] stream  Output file
			/// \param[in] r       Record
			///
			/// \returns The stream \p stream
			///
			friend stdex::stream::basic_file& operator <<(_In_ stdex::stream::basic_file& stream, _In_ const record<T, T_ID, ID, T_SIZE, ALIGN> r)
			{
				// Parameter r does not need to be passed by reference. It has only one field (data), which is a reference itself already.

				auto start = r.open(stream);
				if (!stream.ok()) _Unlikely_ return stream;
				stream << r.data;
				r.close(stream, start);

				return stream;
			}

			///
			/// Writes record to a stream
			///
			/// \param[in] stream  Output stream
			/// \param[in] r       Record
			///
			/// \returns The stream \p stream
			///
			friend stdex::stream::basic& operator <<(_In_ stdex::stream::basic& stream, _In_ const record<T, T_ID, ID, T_SIZE, ALIGN> r)
			{
				// Parameter r does not need to be passed by reference. It has only one field (data), which is a reference itself already.

				stdex::stream::memory_file temp;
				auto start = r.open(temp);
				if (!temp.ok()) _Unlikely_ return stream;
				temp << r.data;
				r.close(temp, start);
				temp.seek(0);
				stream.write_stream(temp);

				return stream;
			}

			///
			/// Reads record from a stream
			///
			/// \param[in]  stream  Input stream
			/// \param[out] r       Record
			///
			/// \returns The stream \p stream
			///
			friend std::istream& operator >>(_In_ std::istream& stream, _In_ record<T, T_ID, ID, T_SIZE, ALIGN> r)
			{
				// Parameter r does not need to be passed by reference. It has only one field (data), which is a reference itself already.

				// Read data size.
				T_SIZE size;
				stream.read((char*)&size, sizeof(size));
				if (!stream.good()) _Unlikely_ return stream;

				// Read data.
				std::streamoff start = stream.tellg();
				stream >> r.data; // TODO: operator >> should not read past the record data! Make a size limited stream and read from it instead.
				if (!stream.good()) _Unlikely_ return stream;

				size += padding<T_SIZE, ALIGN>(size);
				stream.seekg(start + size);

				return stream;
			}

			///
			/// Reads record from a file
			///
			/// \param[in]  stream  Input file
			/// \param[out] r       Record
			///
			/// \returns The stream \p stream
			///
			friend stdex::stream::basic_file& operator >>(_In_ stdex::stream::basic_file& stream, _In_ record<T, T_ID, ID, T_SIZE, ALIGN> r)
			{
				// Parameter r does not need to be passed by reference. It has only one field (data), which is a reference itself already.

				// Read data size.
				T_SIZE size;
				stream >> size;
				if (!stream.ok()) _Unlikely_ return stream;

				// Read data.
				auto start = stream.tell();
				{
					stdex::stream::limiter limiter(stream, size, 0);
					limiter >> r.data;
					if (limiter.state() == stdex::stream::state_t::fail) _Unlikely_ return stream;
				}

				size += padding<T_SIZE, ALIGN>(size);
				stream.seek(start + size);

				return stream;
			}

			///
			/// Reads record from a stream
			///
			/// \param[in]  stream  Input stream
			/// \param[out] r       Record
			///
			/// \returns The stream \p stream
			///
			friend stdex::stream::basic& operator >>(_In_ stdex::stream::basic& stream, _In_ record<T, T_ID, ID, T_SIZE, ALIGN> r)
			{
				// Parameter r does not need to be passed by reference. It has only one field (data), which is a reference itself already.

				// Read data size.
				T_SIZE size;
				stream >> size;
				if (!stream.ok()) _Unlikely_ return stream;

				{
					stdex::stream::limiter limiter(stream, size, 0);
					limiter >> r.data;
					if (limiter.state() == stdex::stream::state_t::fail) _Unlikely_ return stream;
					limiter.skip(limiter.read_limit);
				}
				stream.skip(padding<T_SIZE, ALIGN>(size));

				return stream;
			}
		};
	};
};
