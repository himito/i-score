#pragma once
#include <QObject>
#include <QSettings>
#include <iscore_lib_base_export.h>
#include <iscore/command/SettingsCommand.hpp>
#include <iscore/tools/Todo.hpp>

namespace iscore
{
    class ISCORE_LIB_BASE_EXPORT SettingsDelegateModel : public QObject
    {
        public:
            using QObject::QObject;
            virtual ~SettingsDelegateModel();
    };

    template<typename Parameter>
    struct SettingsParameterMetadata
    {
        public:
            using parameter_type = Parameter;
            using model_type = typename Parameter::model_type;
            using data_type = typename Parameter::param_type;
            using argument_type = typename boost::call_traits<data_type>::param_type;
            QString key;
            data_type def;
    };

    template<typename T>
    using sp = SettingsParameterMetadata<T>;


    template<typename T, typename Model>
    void setupDefaultSettings(QSettings& set, const T& tuple, Model& model)
    {
        for_each_in_tuple(tuple, [&] (auto& e) {
            using type = std::remove_reference_t<decltype(e)>;
            using data_type = typename type::data_type;
            using param_type = typename type::parameter_type;

            // If we cannot find the key, it means that it's a new setting.
            // Hence we set the default value both in the QSettings and in the model.
            if(!set.contains(e.key))
            {
                set.setValue(e.key, QVariant::fromValue(e.def));
                (model.*param_type::set())(e.def);
            }
            else
            {
                // We fetch the value from the settings.
                auto val = set.value(e.key).template value<data_type>();
                (model.*param_type::set())(val);
            }
        });
    }
}

#define ISCORE_SETTINGS_COMMAND(ModelType, Name) \
    struct Set ## ModelType ## Name : public iscore::SettingsCommand<ModelType ## Name ## Parameter> \
{ \
 static constexpr const bool is_deferred = false; \
 ISCORE_SETTINGS_COMMAND_DECL(Set ## ModelType ## Name) \
};

#define ISCORE_SETTINGS_PARAMETER(ModelType, Name) \
    ISCORE_PARAMETER_TYPE(ModelType, Name) \
    ISCORE_SETTINGS_COMMAND(ModelType, Name)



#define ISCORE_SETTINGS_DEFERRED_COMMAND(ModelType, Name) \
    struct Set ## ModelType ## Name : public iscore::SettingsCommand<ModelType ## Name ## Parameter> \
{ \
 static constexpr const bool is_deferred = true; \
 ISCORE_SETTINGS_COMMAND_DECL(Set ## ModelType ## Name) \
};

#define ISCORE_SETTINGS_DEFERRED_PARAMETER(ModelType, Name) \
    ISCORE_PARAMETER_TYPE(ModelType, Name) \
    ISCORE_SETTINGS_DEFERRED_COMMAND(ModelType, Name)



#define ISCORE_SETTINGS_PARAMETER_HPP(Type, Name) \
    public: \
        Type get ## Name() const; \
        void set ## Name(Type); \
        Q_SIGNAL void Name ## Changed(Type) ; \
    private:

#define ISCORE_SETTINGS_PARAMETER_CPP(Type, ModelType, Name) \
Type ModelType :: get ## Name() const \
{ \
    return m_ ## Name; \
} \
 \
void ModelType :: set ## Name(Type val) \
{ \
    if(val == m_ ## Name) \
        return; \
 \
    m_ ## Name = val; \
 \
    QSettings s; \
    s.setValue(Parameters::Name.key, QVariant::fromValue(m_ ## Name)); \
    emit Name ## Changed(val); \
}
