<?xml version="1.0"?>

<!--

Copyright 2009 VMware, Inc.
Copyright 2008 Tungsten Graphics, Inc.

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published
by the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

!-->

<xsl:transform version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:output method="html" />

	<xsl:strip-space elements="*" />

	<xsl:template match="/trace">
		<html>
			<head>
				<title>D3D Trace</title>
			</head>
			<style>
				body {
					font-family: verdana, sans-serif;
					font-size: 11px;
					font-weight: normal;
					text-align : left;
				}

				ul.calls {
					list-style: none;
					margin-left: 0px;
					padding-left: 0px;
				}

				ul.args {
					display:inline;
					list-style: none;
					margin-left: 0px;
					padding-left: 0px;
				}

				ul.args li {
					display:inline;
				}

				ul.elems {
					list-style: none;
					margin-left: 2em;
					padding-left: 0px;
				}

				ul.elems li {
					display:block;
				}

				.fun {
					font-weight: bold;
				}

				.var {
					font-style: italic;
				}

				.typ {
					display: none;
				}

				.lit {
					color: #0000ff;
				}

				.addr {
					color: #008000;
				}

				.ref {
					position: relative;
					cursor: help;
				}

				.def {
					display: none;
					position: absolute;
					top: 1.5em;
					left: 0;
					width: 32em;
					background: #f7f7f7;
					cursor: default;
				}

				.ref:hover .def {
					display: block;
				}
			</style>
			<body>
				<ul class="calls">
					<xsl:apply-templates/>
				</ul>
			</body>
		</html>
	</xsl:template>

	<xsl:template match="call">
		<li>
			<span class="fun">
				<xsl:value-of select="@name"/>
			</span>
			<xsl:text>(</xsl:text>
			<ul class="args">
				<xsl:apply-templates select="arg"/>
			</ul>
			<xsl:text>)</xsl:text>
			<xsl:apply-templates select="ret"/>
		</li>
	</xsl:template>

	<xsl:template match="arg|elem">
		<li>
			<xsl:apply-templates select="@type"/>
			<xsl:apply-templates select="@name"/>
			<xsl:text> = </xsl:text>
			<xsl:call-template name="compound"/>
			<xsl:if test="position() != last()">
				<xsl:text>, </xsl:text>
			</xsl:if>
		</li>
	</xsl:template>

	<xsl:template match="@type">
		<xsl:attribute name="title">
			<xsl:value-of select="."/>
		</xsl:attribute>
	</xsl:template>

	<xsl:template match="@name">
		<span class="var">
			<xsl:value-of select="."/>
		</span>
	</xsl:template>

	<xsl:template match="ret">
		<xsl:text> = </xsl:text>
		<xsl:call-template name="compound"/>
	</xsl:template>

	<xsl:template match="ref">
		<xsl:choose>
			<xsl:when test="elem">
				<span class="ref">
					<xsl:apply-templates select="@addr"/>
					<span class="def">
						<xsl:call-template name="compound"/>
					</span>
				</span>
			</xsl:when>
			<xsl:when test="*">
				<xsl:text>&amp;</xsl:text>
				<xsl:apply-templates />
			</xsl:when>
			<xsl:otherwise>
				<xsl:apply-templates select="@addr"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	<xsl:template match="@addr">
		<span class="addr">
			<xsl:value-of select="."/>
		</span>
	</xsl:template>

	<xsl:template match="text()">
		<span class="lit">
			<xsl:value-of select="."/>
		</span>
	</xsl:template>

	<xsl:template name="compound">
		<xsl:choose>
			<xsl:when test="elem">
				<xsl:text>{</xsl:text>
				<ul class="elems">
					<xsl:apply-templates />
				</ul>
				<xsl:text>}</xsl:text>
			</xsl:when>
			<xsl:otherwise>
				<xsl:apply-templates />
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
</xsl:transform>
