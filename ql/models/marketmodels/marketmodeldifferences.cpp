/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2007 Fran�ois du Vignaud

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/



#include <ql/models/marketmodels/marketmodel.hpp>
#include <ql/models/marketmodels/evolutiondescription.hpp>


namespace QuantLib {

    std::vector<Volatility> rateVolDifferences(
                                           const MarketModel& marketModel1,
                                           const MarketModel& marketModel2) {
        QL_ENSURE(marketModel1.initialRates() == marketModel2.initialRates(),
                  "initialRates do not match");
        const EvolutionDescription& evolutionDescription1 
                                           = marketModel1.evolution();
        const EvolutionDescription& evolutionDescription2 
                                           = marketModel2.evolution();
        QL_ENSURE(evolutionDescription1.evolutionTimes()
                  == evolutionDescription2.evolutionTimes(),
                  "Evolution times do not match");
        
        const Matrix& totalCovariance1 
            = marketModel1.totalCovariance(marketModel1.numberOfSteps()-1);
        const Matrix& totalCovariance2 
            = marketModel2.totalCovariance(marketModel2.numberOfSteps()-1);
        const std::vector<Time>& maturities =
            evolutionDescription1.evolutionTimes();

        std::vector<Volatility> result(totalCovariance1.columns());
        for (Size i=0; i<totalCovariance1.columns(); ++i) {
            Real diff = totalCovariance1[i][i]-totalCovariance2[i][i];
            result[i] = std::sqrt(diff/maturities[i]);
        }
        return result;
    }       

    std::vector<Real> rateInstVolDifferences(
                                           const MarketModel& marketModel1,
                                           const MarketModel& marketModel2, 
                                           Size index) {
        QL_ENSURE(marketModel1.initialRates() == marketModel2.initialRates(),
                  "initialRates do not match");
        const EvolutionDescription& evolutionDescription1
                                           = marketModel1.evolution();
        const EvolutionDescription& evolutionDescription2
                                           = marketModel2.evolution();
        QL_ENSURE(evolutionDescription1.evolutionTimes()
                  == evolutionDescription2.evolutionTimes(),
                  "Evolution times do not match");
        QL_ENSURE(index<evolutionDescription1.numberOfSteps(), 
            "the index given is greater than the number of steps");

        const std::vector<Time>& evolutionTimes 
            = evolutionDescription1.evolutionTimes();
        std::vector<Real> result(evolutionTimes.size());

        Time previousEvolutionTime = 0;
        for (Size i=0; i<evolutionTimes.size(); ++i) {
            Time currentEvolutionTime = evolutionTimes[i];
            Time dt = currentEvolutionTime - previousEvolutionTime;
            const Matrix& covariance1 = marketModel1.covariance(i);
            const Matrix& covariance2 = marketModel2.covariance(i);
            Real diff = covariance1[index][index] - covariance2[index][index];
            result[i] = std::sqrt(diff/dt);
            previousEvolutionTime = currentEvolutionTime;
        }
        return result;
    }

}
