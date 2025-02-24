//
// Created by Matthew McCall on 7/4/23.
//

#include "Oasis/BinaryExpression.hpp"

namespace Oasis {

BinaryExpressionBase<Expression>::BinaryExpressionBase(const BinaryExpressionBase& other)
{
    if (other.mostSigOp) {
        mostSigOp = other.mostSigOp->Copy();
    }

    if (other.leastSigOp) {
        leastSigOp = other.leastSigOp->Copy();
    }
}

BinaryExpressionBase<Expression>::BinaryExpressionBase(const Expression& mostSigOp, const Expression& leastSigOp)
    : mostSigOp { mostSigOp.Copy() }
    , leastSigOp { leastSigOp.Copy() }
{
}

auto BinaryExpressionBase<Expression>::Equals(const Oasis::Expression& other) const -> bool
{
    if (this->GetType() != other.GetType()) {
        return false;
    }

    const auto& otherBinary = dynamic_cast<const BinaryExpressionBase&>(other);

    if ((mostSigOp == nullptr) == (otherBinary.mostSigOp == nullptr)) {
        if (mostSigOp && otherBinary.mostSigOp) {
            if (!mostSigOp->Equals(*otherBinary.mostSigOp)) {
                return false;
            }
        }
    } else {
        return false;
    }

    if ((leastSigOp == nullptr) == (otherBinary.leastSigOp == nullptr)) {
        if (leastSigOp && otherBinary.leastSigOp) {
            if (!leastSigOp->Equals(*otherBinary.leastSigOp)) {
                return false;
            }
        }
    } else {
        return false;
    }

    return true;
}

auto BinaryExpressionBase<Expression>::StructurallyEquivalent(const Expression& other) const -> bool
{
    if (this->GetType() != other.GetType()) {
        return false;
    }

    std::unique_ptr<Expression> otherNormalized = other.Generalize();
    const auto& otherBinary = dynamic_cast<const BinaryExpressionBase&>(*otherNormalized);

    if ((mostSigOp == nullptr) == (otherBinary.mostSigOp == nullptr)) {
        if (mostSigOp && otherBinary.mostSigOp) {
            if (!mostSigOp->StructurallyEquivalent(*otherBinary.mostSigOp)) {
                return false;
            }
        }
    } else {
        return false;
    }

    if ((leastSigOp == nullptr) == (otherBinary.leastSigOp == nullptr)) {
        if (leastSigOp && otherBinary.leastSigOp) {
            if (!leastSigOp->StructurallyEquivalent(*otherBinary.leastSigOp)) {
                return false;
            }
        }
    } else {
        return false;
    }

    return true;
}

auto BinaryExpressionBase<Expression>::StructurallyEquivalent(const Expression& other, tf::Subflow& subflow) const -> bool
{
    if (this->GetType() != other.GetType()) {
        return false;
    }

    std::unique_ptr<Expression> otherGeneralized;

    tf::Task generalizeTask = subflow.emplace([&](tf::Subflow& sbf) {
        otherGeneralized = other.Generalize(sbf);
    });

    bool mostSigOpEquivalent = false, leastSigOpEquivalent = false;

    tf::Task compMostSigOp = subflow.emplace([this, &otherGeneralized, &mostSigOpEquivalent](tf::Subflow& sbf) {
        const auto& otherBinary = dynamic_cast<const BinaryExpressionBase&>(*otherGeneralized);
        if ((mostSigOp == nullptr) == (otherBinary.mostSigOp == nullptr)) {
            if (mostSigOp && otherBinary.mostSigOp) {
                mostSigOpEquivalent = mostSigOp->StructurallyEquivalent(*otherBinary.mostSigOp, sbf);
            }
        } else {
            mostSigOpEquivalent = false;
        }
    });

    compMostSigOp.succeed(generalizeTask);

    tf::Task compLeastSigOp = subflow.emplace([this, &otherGeneralized, &leastSigOpEquivalent](tf::Subflow& sbf) {
        const auto& otherBinary = dynamic_cast<const BinaryExpressionBase&>(*otherGeneralized);
        if ((leastSigOp == nullptr) == (otherBinary.leastSigOp == nullptr)) {
            if (leastSigOp && otherBinary.leastSigOp) {
                leastSigOpEquivalent = leastSigOp->StructurallyEquivalent(*otherBinary.leastSigOp, sbf);
            }
        } else {
            leastSigOpEquivalent = false;
        }
    });

    compLeastSigOp.succeed(generalizeTask);

    subflow.join();

    return mostSigOpEquivalent && leastSigOpEquivalent;
}

auto BinaryExpressionBase<Expression>::AddOperand(const std::unique_ptr<Expression>& operand) -> bool
{
    if (mostSigOp && leastSigOp) {
        return false;
    }

    assert(operand != nullptr);

    if (mostSigOp == nullptr) {
        mostSigOp = operand->Copy();
        return true;
    }

    if (leastSigOp == nullptr) {
        leastSigOp = operand->Copy();
        return true;
    }

    return false;
}

auto BinaryExpressionBase<Expression>::SetMostSigOp(const Expression& op) -> void
{
    mostSigOp = op.Copy();
}

auto BinaryExpressionBase<Expression>::SetLeastSigOp(const Expression& op) -> void
{
    leastSigOp = op.Copy();
}

auto BinaryExpressionBase<Expression>::HasMostSigOp() const -> bool
{
    return mostSigOp != nullptr;
}

auto BinaryExpressionBase<Expression>::HasLeastSigOp() const -> bool
{
    return leastSigOp != nullptr;
}

auto BinaryExpressionBase<Expression>::GetMostSigOp() const -> const Expression&
{
    return *mostSigOp;
}

auto BinaryExpressionBase<Expression>::GetLeastSigOp() const -> const Expression&
{
    return *leastSigOp;
}

auto BinaryExpressionBase<Expression>::operator=(const Oasis::BinaryExpressionBase<Expression>& other) -> BinaryExpressionBase<Expression>&
{
    if (other.mostSigOp) {
        mostSigOp = other.mostSigOp->Copy();
    }

    if (other.leastSigOp) {
        leastSigOp = other.leastSigOp->Copy();
    }

    return *this;
}

} // namespace Oasis