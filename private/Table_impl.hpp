#pragma once
#include "../icedb/Table.hpp"
#include "Attribute_impl.hpp"

namespace icedb {
	namespace Tables {
		class Table_impl : virtual public Table, virtual public Attributes::CanHaveAttributes_impl {
			std::shared_ptr<H5::DataSet> obj;
		protected:
			void _setTableSelf(std::shared_ptr<H5::DataSet> obj) override;
			std::shared_ptr<H5::DataSet> _getTableSelf() const override;
		public:
			virtual ~Table_impl();
			Table_impl(std::shared_ptr<H5::DataSet> obj, const std::string &name = "");
		};

		class CanHaveTables_impl : virtual public CanHaveTables {
			std::shared_ptr<H5::Group> obj;
		protected:
			void _setTablesParent(std::shared_ptr<H5::Group> obj) override;
			std::shared_ptr<H5::Group> _getTablesParent() const override;
		public:
			virtual ~CanHaveTables_impl();
			CanHaveTables_impl();
		};
	}
}