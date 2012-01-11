/*
    libKGoogle - Account
    Copyright (C) 2011  Dan Vratil <dan@progdan.cz>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "account.h"

using namespace KGoogle;

Account::Account(const QString &accName, const QString &accessToken,
                 const QString &refreshToken, const QList< QUrl > &scopes):
  m_accName(accName),
  m_accessToken(accessToken),
  m_refreshToken(refreshToken),
  m_scopes(scopes),
  m_scopesChanged(false)
{

}

Account::~Account()
{

}

QString Account::accountName() const
{
  return m_accName;
}

void Account::setAccountName(const QString& accountName)
{
  m_accName = accountName;
}

const QString &Account::accessToken() const
{
  return m_accessToken;
}

void Account::setAccessToken (const QString &accessToken)
{
  m_accessToken = accessToken;
}

const QString &Account::refreshToken() const
{
  return m_refreshToken;
}

void Account::setRefreshToken (const QString &refreshToken)
{
  m_refreshToken = refreshToken;
}

const QList< QUrl > &Account::scopes() const
{
  return m_scopes;
}

void Account::setScopes (const QList< QUrl > &scopes)
{
  m_scopes = scopes;
  m_scopesChanged = true;
}

void Account::addScope (const QUrl &scope)
{
  if (!m_scopes.contains(scope)) {
    m_scopes.append(scope);
    m_scopesChanged = true;
  }
}

void Account::removeScope (const QUrl &scope)
{
  if (m_scopes.contains(scope)) {
    m_scopes.removeOne(scope);
    m_scopesChanged = true;
  }
}









