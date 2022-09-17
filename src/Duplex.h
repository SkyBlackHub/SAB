#ifndef DUPLEX_H
#define DUPLEX_H

#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QRadioButton>
#include <QPlainTextEdit>

namespace SAB {

	template <typename Type, typename Func1, typename Func2>
	static inline void duplex(QLineEdit *edit, Type *target, Func1 signal, Func2 slot, const QString &value = QString())
	{
		edit->setText(value);
		QObject::connect(target, signal, edit, [edit] (const QString &value) {
			if (edit->updatesEnabled() == false) {
				return;
			}
			edit->setUpdatesEnabled(false);
			edit->setText(value);
			edit->setUpdatesEnabled(true);
		});
		QObject::connect(edit, &QLineEdit::textChanged, target, [target, slot, edit] (const QString &value) {
			if (edit->updatesEnabled() == false) {
				return;
			}
			edit->setUpdatesEnabled(false);
			(target->*(slot))(value);
			edit->setUpdatesEnabled(true);
		});
	}

	template <typename Type, typename Func1, typename Func2>
	static inline void duplex(QSpinBox *spin, Type *target, Func1 signal, Func2 slot, int value)
	{
		spin->setValue(value);
		QObject::connect(target, signal, spin, &QSpinBox::setValue);
		QObject::connect(spin, &QSpinBox::valueChanged, target, slot);
	}

	template <typename Type, typename Func1, typename Func2>
	static inline void duplex(QCheckBox *check, Type *target, Func1 signal, Func2 slot, bool value)
	{
		check->setChecked(value);
		QObject::connect(target, signal, check, &QCheckBox::setChecked);
		QObject::connect(check, &QCheckBox::toggled, target, slot);
	}

	template <typename Type, typename Func1, typename Func2>
	static inline void duplex(QAction *action, Type *target, Func1 signal, Func2 slot, bool value)
	{
		action->setChecked(value);
		QObject::connect(target, signal, action, &QAction::setChecked);
		QObject::connect(action, &QAction::toggled, target, slot);
	}

	template <typename Type, typename Func1, typename Func2>
	static inline void duplex(QPushButton *button, Type *target, Func1 signal, Func2 slot, bool value)
	{
		button->setChecked(value);
		QObject::connect(target, signal, button, &QCheckBox::setChecked);
		QObject::connect(button, &QCheckBox::toggled, target, slot);
	}

	template <typename Type, typename Func1, typename Func2>
	static inline void duplex(QPushButton *button_true, QPushButton *button_false, Type *target, Func1 signal, Func2 slot, bool value)
	{
		if (value) {
			button_true->setChecked(true);
		} else {
			button_false->setChecked(true);
		}
		QObject::connect(button_true, &QPushButton::toggled, target, slot);
		QObject::connect(target, signal, target, [button_true, button_false] (bool value) {
			if (value) {
				button_true->setChecked(true);
			} else {
				button_false->setChecked(true);
			}
		});
	}

	template <typename Type, typename Func1, typename Func2, typename ValueType>
	static inline void duplex(const QHash<ValueType, QRadioButton *> &values, Type *target, Func1 signal, Func2 slot, ValueType value)
	{
		values[value]->setChecked(true);

		QObject::connect(target, signal, target, [values] (ValueType value) {
			values[value]->setChecked(true);
		});

		QHashIterator<ValueType, QRadioButton *> iterator(values);

		while (iterator.hasNext()) {
			iterator.next();
			ValueType value = iterator.key();
			QObject::connect(iterator.value(), &QRadioButton::toggled, target, [target, slot, value] (bool checked) {
				if (checked) {
					(target->*(slot))(value);
				}
			});
		}
	}

	template <typename Type, typename Func1, typename Func2>
	static inline void duplex(QPlainTextEdit *edit, Type *target, Func1 signal, Func2 slot, const QStringList &value = QStringList())
	{
		edit->setPlainText(value.join('\n'));
		QObject::connect(target, signal, edit, [edit] (const QStringList &value) {
			if (edit->updatesEnabled() == false) {
				return;
			}
			edit->setUpdatesEnabled(false);
			edit->setPlainText(value.join('\n'));
			edit->setUpdatesEnabled(true);
		});
		QObject::connect(edit, &QPlainTextEdit::textChanged, target, [target, slot, edit] () {
			if (edit->updatesEnabled() == false) {
				return;
			}
			edit->setUpdatesEnabled(false);
			(target->*(slot))(edit->toPlainText().split('\n'));
			edit->setUpdatesEnabled(true);
		});
	}
}

#endif // DUPLEX_H
