/**
 * @file transformer.cpp
 * @author Chase Geigle
 */

#include <unicode/translit.h>

#include "detail.h"
#include "utf/transformer.h"
#include "util/pimpl.tcc"

namespace meta
{
namespace utf
{

/**
 * Implementation class for the transformer.
 */
class transformer::impl
{
  public:
    impl(const std::string& id)
    {
        icu_handle::get();
        auto icu_id = icu::UnicodeString::fromUTF8(id);
        auto status = U_ZERO_ERROR;
        translit_.reset(icu::Transliterator::createInstance(
            icu_id, UTRANS_FORWARD, status));
        if (!translit_ || !U_SUCCESS(status))
            throw std::runtime_error{"failed to create transformer"};
    }

    impl(const impl& other) : translit_{other.translit_->clone()}
    {
        // nothing
    }

    impl(impl&&) = default;

    std::string convert(const std::string& str)
    {
        auto icu_str = icu::UnicodeString::fromUTF8(str);
        translit_->transliterate(icu_str);
        return icu_to_u8str(icu_str);
    }

  private:
    std::unique_ptr<icu::Transliterator> translit_;
};

transformer::transformer(const std::string& id) : impl_{id}
{
    icu_handle::get();
}

transformer::transformer(const transformer& other) : impl_{*other.impl_}
{
    // nothing
}

transformer::transformer(transformer&&) = default;

transformer::~transformer() = default;

std::string transformer::operator()(const std::string& str)
{
    return impl_->convert(str);
}

std::string transform(const std::string& str, const std::string& id)
{
    icu_handle::get();
    transformer trans{id};
    return trans(str);
}
}
}
